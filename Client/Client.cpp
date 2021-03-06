#include "stdafx.h"
#include "RakSleep.h"

#include <Kbhit.h>
#include "Gets.h"

#include "SirenaPackets.h"

#define SERVER_PORT 42069

int main(void)
{
	VersionInfo versionInfo;

	RakNet::RakPeerInterface* rakPeer = RakNet::RakPeerInterface::GetInstance();
	RakNet::Packet* packet;

	std::cout << "+==================+" << std::endl;
	std::cout << "Sirena client v" << int(versionInfo.major) << '.' << int(versionInfo.minor) << std::endl;
	std::cout << "+==================+" << std::endl << std::endl;

	RakNet::SocketDescriptor sd;
	rakPeer->Startup(1, &sd, 1);
	RakNet::SystemAddress serverSysAddr;

	std::cout << "Enter server IP or hit enter for localhost." << std::endl;
	char str[128];
	gets_s(str);

	if (str[0] == 0) {
		strcpy_s(str, "127.0.0.1");
	}

	rakPeer->Connect(str, SERVER_PORT, "4.081 LBB Sirena", 16);

	while (true) {
		RakSleep(80);

		if (_kbhit())
		{
			//Comment from Rakkar: 
			// Notice what is not here: something to keep our network running.  It's
			// fine to block on gets or anything we want
			// Because the network engine was painstakingly written using threads.
			char message[2048];
			Gets(message, sizeof(message));

			if (strcmp(message, "quit") == 0)
			{
				std::cout << "Quitting" << std::endl;
				break;
			}

			RakNet::RakString rsMessage{ message };
			RakNet::BitStream bitStream;
			bitStream.Write(RakNet::MessageID(ID_SIRENA_GENERAL_CHAT_MESSAGE));
			bitStream.Write(rsMessage);
			rakPeer->Send(&bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, serverSysAddr, false);
		}

		for (packet = rakPeer->Receive(); packet; rakPeer->DeallocatePacket(packet), packet = rakPeer->Receive()) {
			switch (packet->data[0]) {
			case ID_REMOTE_DISCONNECTION_NOTIFICATION:
				std::cout << "Another client has disconnected." << std::endl;
				break;

			case ID_REMOTE_CONNECTION_LOST:
				std::cout << "Another client has lost connection." << std::endl;
				break;

			case ID_REMOTE_NEW_INCOMING_CONNECTION:
				std::cout << "Another client has connected." << std::endl;
				break;

			case ID_CONNECTION_REQUEST_ACCEPTED: {
				std::cout << "Our connection request was accepted." << std::endl;
				SirenaPackets::SendVersionConfirm(rakPeer, packet, versionInfo);
				serverSysAddr = packet->systemAddress;
				break;
			}

			case ID_NEW_INCOMING_CONNECTION:
				std::cout << "A new connection is incoming." << std::endl;
				break;

			case ID_NO_FREE_INCOMING_CONNECTIONS:
				std::cout << "\033[1;31mThe server is full!\033[0m" << std::endl;
				break;

			case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "\033[1;31mYou have been disconnected from the server.\033[0m" << std::endl;
				break;

			case ID_CONNECTION_LOST:
				std::cout << "\033[1;31mYou have lost the connection to the server.\033[0m" << std::endl;
				break;

			case ID_SIRENA_VERSION_ACCEPTED: {
				std::cout << "Our version was accepted by the server." << std::endl;
#ifdef WIN32
				system("cls");
#else
				system("reset");
#endif
				SirenaPackets::HandleVersionAccepted(packet);
				break;
			}

			case ID_SIRENA_DISCONNECTION_NOTIFICATION: {
				SirenaPackets::HandleDisconnectionNotification(rakPeer, packet);
				break;
			}

			case ID_SIRENA_MOTD: {
				SirenaPackets::HandleMOTD(rakPeer, packet);
				break;
			}

			case ID_SIRENA_SET_NAME: {
				SirenaPackets::HandleSetName(packet);
				break;
			}

			case ID_SIRENA_GENERAL_CHAT_MESSAGE: {
				SirenaPackets::HandleGeneralChatMessage(rakPeer, packet);
				break;
			}

			default:
				std::cout << "Message with identifier " << int(packet->data[0]) << " has arrived." << std::endl;
				break;
			}
		}
	}

	RakNet::RakPeerInterface::DestroyInstance(rakPeer);
    return 0;
}
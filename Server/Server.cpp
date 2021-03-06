#include "stdafx.h"
#include "RakSleep.h"
#include <Kbhit.h>
#include "Gets.h"
#include "RakNetStatistics.h"
#include "SirenaPackets.h"
#include "tinyxml2.h"
#include "UserPool.h"
#include "Singleton.h"

//Temp until the config can be stored elsewhere:
#define MAX_CLIENTS 10
#define SERVER_PORT 42069

void LoadServerConfig(ServerConfig& config);

int main(void)
{
	RakNet::RakPeerInterface* rakPeer = RakNet::RakPeerInterface::GetInstance();
	RakNet::RakNetStatistics* rss;
	RakNet::Packet* packet;
	VersionInfo versionInfo;
	Singleton<UserPool> userspool{};

	ServerConfig config{};
	LoadServerConfig(config);

	std::vector<ChannelInfo*> channels;
	std::vector<Rank*> ranks;
	std::vector<User*> users;
	std::vector<std::pair<std::string, std::string>> serverMessages;

	std::cout << "+==================+" << std::endl;
	std::cout << "Sirena server v" << (int)versionInfo.major << '.' << (int)versionInfo.minor << std::endl;
	std::cout << "+==================+" << std::endl << std::endl;

	RakNet::SocketDescriptor sd(SERVER_PORT, 0);
	rakPeer->Startup(MAX_CLIENTS, &sd, 1);

	std::cout << "Starting as server." << std::endl;
	rakPeer->SetMaximumIncomingConnections(MAX_CLIENTS);
	rakPeer->SetIncomingPassword("4.081 LBB Sirena", 16);

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

			if (strcmp(message, "stats") == 0)
			{
				rss = rakPeer->GetStatistics(rakPeer->GetSystemAddressFromIndex(0));
				RakNet::StatisticsToString(rss, message, 2);
				printf("%s", message);
				printf("Ping %i\n", rakPeer->GetAveragePing(rakPeer->GetSystemAddressFromIndex(0)));

				continue;
			}

			if (strcmp(message, "kick") == 0)
			{
				std::cout << "Kicking is not implemented yet." << std::endl;

				continue;
			}

			if (strcmp(message, "getconnectionlist") == 0)
			{
				RakNet::SystemAddress systems[10];
				unsigned short numConnections = 10;
				rakPeer->GetConnectionList((RakNet::SystemAddress*) &systems, &numConnections);
				for (int i = 0; i < numConnections; i++)
				{
					printf("%i. %s\n", i + 1, systems[i].ToString(true));
				}
				continue;
			}

			if (strcmp(message, "ban") == 0)
			{
				printf("Enter IP to ban.  You can use * as a wildcard\n");
				Gets(message, sizeof(message));
				rakPeer->AddToBanList(message);
				printf("IP %s added to ban list.\n", message);

				continue;
			}
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
				break;
			}

			case ID_NEW_INCOMING_CONNECTION:
				std::cout << "A new connection is incoming." << std::endl;
				break;

			case ID_NO_FREE_INCOMING_CONNECTIONS:
				std::cout << "The server is full!" << std::endl;
				break;

			case ID_DISCONNECTION_NOTIFICATION:
				std::cout << "A client has been disconnected." << std::endl;
				break;

			case ID_CONNECTION_LOST:
				std::cout << "A client has lost the connection." << std::endl;
				break;

			case ID_SIRENA_VERSION_CONFIRM: {
				SirenaPackets::HandleVersionConfirm(rakPeer, packet, versionInfo, config.motd, config.serverName);
				break;
			}
			case ID_SIRENA_GENERAL_CHAT_MESSAGE: {
				SirenaPackets::HandleGeneralChatMessageServer(rakPeer, packet);
				break;
			}

			default:
				std::cout << "Message with identifier " << int(packet->data[0]) << " has arrived." << std::endl;
				break;
			}
		}
	}

	rakPeer->Shutdown(300);
	RakNet::RakPeerInterface::DestroyInstance(rakPeer);
	return 0;
}

void LoadServerConfig(ServerConfig & config)
{
	const char* sbuf;
	tinyxml2::XMLDocument doc{};
	doc.LoadFile("./ServerConfig.xml");
	tinyxml2::XMLElement* element = doc.FirstChildElement()->FirstChildElement("Name");

	element->QueryStringAttribute("value", &sbuf);
	config.serverName = sbuf;

	element = doc.FirstChildElement()->FirstChildElement("Port");
	element->QueryUnsignedAttribute("value", &config.serverPort);

	element = doc.FirstChildElement()->FirstChildElement("Motd");
	element->QueryStringAttribute("value", &sbuf);
	config.motd = sbuf;

	element = doc.FirstChildElement()->FirstChildElement("Token");
	element->QueryStringAttribute("value", &sbuf);
	config.serverToken = sbuf;

	element = doc.FirstChildElement()->FirstChildElement("MaxUsers");
	element->QueryUnsignedAttribute("value", &config.maxUsers);

	//Load in the channels:

}

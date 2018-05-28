#include "stdafx.h"
#include "SirenaPackets.h"
#include "Singleton.h"
#include "UserPool.h"
#include <sstream>

void SirenaPackets::SendVersionConfirm(RakNet::RakPeerInterface * rakPeer, RakNet::Packet * packet, VersionInfo & versionInfo)
{
	RakNet::BitStream bitStream;
	bitStream.Write((RakNet::MessageID)ID_SIRENA_VERSION_CONFIRM);
	bitStream.Write(versionInfo.major); //Server will deny entry if this differs
	bitStream.Write(versionInfo.minor);
	rakPeer->Send(&bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
}

void SirenaPackets::HandleVersionConfirm(RakNet::RakPeerInterface * rakPeer, RakNet::Packet * packet, VersionInfo & ourVersionInfo, RakNet::RakString& motd, RakNet::RakString& serverName)
{
	VersionInfo theirVersion;

	RakNet::BitStream in(packet->data, packet->length, false);
	in.IgnoreBytes(sizeof(RakNet::MessageID));
	in.Read(theirVersion.major);
	in.Read(theirVersion.minor);
	std::cout << "Received version confirm with: " << (int)theirVersion.major << '.' << (int)theirVersion.minor << std::endl;

	if (theirVersion.major == ourVersionInfo.major)
	{
		SirenaPackets::SendVersionAccepted(rakPeer, packet, serverName);
		SirenaPackets::SendMOTD(rakPeer, packet, motd);
	}
	else
	{
		SirenaPackets::SendDisconnectionNotification(rakPeer, packet, SirenaDisconnectionNotififiers::VERSION_MISMATCH, ourVersionInfo);
	}
}

void SirenaPackets::HandleVersionAccepted(RakNet::Packet * packet)
{
	RakNet::RakString serverName;
	RakNet::BitStream in(packet->data, packet->length, false);
	in.IgnoreBytes(sizeof(RakNet::MessageID));
	in.Read(serverName);

	std::cout << "Connected to: " << serverName.C_String() << std::endl << std::endl;
}

void SirenaPackets::SendVersionAccepted(RakNet::RakPeerInterface * rakPeer, RakNet::Packet * packet, RakNet::RakString& serverName)
{
	Singleton<UserPool> userPool;
	User* user = new User();
	std::string tempname = "Guest" + std::to_string(rand());
	user->name = tempname.c_str();
	user->rank = 1; //TODO: Check the rank first.
	userPool.instance.Insert(packet->systemAddress, user);

	RakNet::BitStream bitStream;
	bitStream.Write(RakNet::MessageID(ID_SIRENA_VERSION_ACCEPTED));
	bitStream.Write(serverName);
	rakPeer->Send(&bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
}

void SirenaPackets::HandleDisconnectionNotification(RakNet::RakPeerInterface * rakPeer, RakNet::Packet * packet)
{
	RakNet::BitStream in(packet->data, packet->length, false);
	in.IgnoreBytes(sizeof(RakNet::MessageID));
	SirenaDisconnectionNotififiers reason;
	in.Read(reason);

	switch (reason) {
	case SirenaDisconnectionNotififiers::VERSION_MISMATCH:
		unsigned int serverMajor;
		in.Read(serverMajor);
		std::cout << "\033[1;31mYour client is not on the same version as the server! Server's version: " << serverMajor << "\033[0m" << std::endl;
		break;
	case SirenaDisconnectionNotififiers::BANNED:
		std::cout << "\033[1;31mYou are banned from this server.\033[0m" << std::endl;
		break;
	}
}

void SirenaPackets::SendDisconnectionNotification(RakNet::RakPeerInterface * rakPeer, RakNet::Packet * packet, SirenaDisconnectionNotififiers notif, VersionInfo& versionInfo)
{
	RakNet::BitStream bitStream;
	bitStream.Write(RakNet::MessageID(ID_SIRENA_DISCONNECTION_NOTIFICATION));
	bitStream.Write(uint32_t(notif));

	switch (notif) {
	case SirenaDisconnectionNotififiers::VERSION_MISMATCH:
		bitStream.Write(versionInfo.major);
		break;
	}

	rakPeer->Send(&bitStream, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
}

void SirenaPackets::HandleMOTD(RakNet::RakPeerInterface * rakPeer, RakNet::Packet * packet)
{
	RakNet::BitStream in(packet->data, packet->length, false);
	in.IgnoreBytes(sizeof(RakNet::MessageID));
	RakNet::RakString motd;
	in.Read(motd);
	std::cout << "\033[1;33m<Server> MOTD: " << motd.C_String() << "\033[0m" << std::endl;
}

void SirenaPackets::SendMOTD(RakNet::RakPeerInterface * rakPeer, RakNet::Packet * packet, RakNet::RakString& motd)
{
	RakNet::BitStream bs;
	bs.Write(RakNet::MessageID(ID_SIRENA_MOTD));
	bs.Write(motd);
	rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, false);
}

void SirenaPackets::HandleSetName(RakNet::Packet * packet)
{
	RakNet::BitStream in(packet->data, packet->length, false);
	in.IgnoreBytes(sizeof(RakNet::MessageID));
	RakNet::RakString oldName;
	RakNet::RakString newName;
	in.Read(oldName);
	in.Read(newName);
	std::cout << "\033[1;33m<Server> " << oldName.C_String() << " is now known as: " << newName.C_String() << "\033[0m" << std::endl;
}

void SirenaPackets::SendSetName(RakNet::RakPeerInterface * rakPeer, const RakNet::RakString& oldName, const RakNet::RakString & newName)
{
	RakNet::BitStream bs;
	bs.Write(RakNet::MessageID(ID_SIRENA_SET_NAME));
	bs.Write(oldName);
	bs.Write(newName);
	rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, RakNet::UNASSIGNED_SYSTEM_ADDRESS, true);
}

//This is the CLIENT SIDED VERSION!!
void SirenaPackets::HandleGeneralChatMessage(RakNet::RakPeerInterface * rakPeer, RakNet::Packet * packet)
{
	RakNet::BitStream in(packet->data, packet->length, false);
	in.IgnoreBytes(sizeof(RakNet::MessageID));
	
	RakNet::RakString message;
	in.Read(message);

	std::cout << message.C_String() << std::endl;
}

void SirenaPackets::HandleGeneralChatMessageServer(RakNet::RakPeerInterface * rakPeer, RakNet::Packet * packet)
{
	Singleton<UserPool> userPool;
	User* user = userPool.instance.Find(packet->systemAddress);

	RakNet::RakString message;
	RakNet::BitStream in(packet->data, packet->length, false);
	in.IgnoreBytes(sizeof(RakNet::MessageID));
	in.Read(message);

	std::string sMessage{ message.C_String() };
	if (sMessage[0] == '/')
	{
		std::cout << "User tried to execute: " << sMessage << std::endl;
		
		std::stringstream temp(sMessage);
		std::string segment;
		std::vector<std::string> seglist;

		while (std::getline(temp, segment, ' ')) seglist.push_back(segment);
		if (seglist[0] == "/nick" && seglist.size() == 2)
		{
			SirenaPackets::SendSetName(rakPeer, user->name, RakNet::RakString(seglist[1].c_str()));
			user->name = RakNet::RakString(seglist[1].c_str());
		}
	}
	else
	{
		RakNet::RakString rsMessage = "<" + user->name + "> " + message;
		RakNet::BitStream bs;
		bs.Write(RakNet::MessageID(ID_SIRENA_GENERAL_CHAT_MESSAGE));
		bs.Write(rsMessage);
		rakPeer->Send(&bs, HIGH_PRIORITY, RELIABLE_ORDERED, 0, packet->systemAddress, true);
	}
}

void SirenaPackets::HandleGetChannels(RakNet::RakPeerInterface * rakPeer, RakNet::Packet * packet)
{
}

void SirenaPackets::SendChannelList(RakNet::RakPeerInterface * rakPeer, RakNet::Packet * packet, std::vector<ChannelInfo*> channels)
{
}

void SirenaPackets::HandleChangeChannels(RakNet::RakPeerInterface * rakPeer, RakNet::Packet * packet)
{
}

void SirenaPackets::SendChangeChannelsResponse(RakNet::RakPeerInterface * rakPeer, RakNet::Packet * packet, ChangeChannelResponse response, ChannelInfo newChannelInfo)
{
}

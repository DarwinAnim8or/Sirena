#pragma once
#include "../RakNet-master/Source/RakNetDefines.h"
#include "../RakNet-master/Source/RakPeerInterface.h"
#include "../RakNet-master/Source/RakString.h"
#include "SirenaDefines.h"
#include <string>
#include <vector>

namespace SirenaPackets {
	void SendVersionConfirm(RakNet::RakPeerInterface* rakPeer, RakNet::Packet* packet, VersionInfo& versionInfo);
	void HandleVersionConfirm(RakNet::RakPeerInterface* rakPeer, RakNet::Packet* packet, VersionInfo& ourVersionInfo, RakNet::RakString& motd, RakNet::RakString& serverName);
	
	void HandleVersionAccepted(RakNet::Packet* packet);
	void SendVersionAccepted(RakNet::RakPeerInterface* rakPeer, RakNet::Packet* packet, RakNet::RakString& serverName);
	
	void HandleDisconnectionNotification(RakNet::RakPeerInterface* rakPeer, RakNet::Packet* packet);
	void SendDisconnectionNotification(RakNet::RakPeerInterface* rakPeer, RakNet::Packet* packet, SirenaDisconnectionNotififiers notif, VersionInfo& versionInfo);
	
	void HandleMOTD(RakNet::RakPeerInterface* rakPeer, RakNet::Packet* packet);
	void SendMOTD(RakNet::RakPeerInterface* rakPeer, RakNet::Packet* packet, RakNet::RakString& motd);
	
	void HandleSetName(RakNet::Packet* packet);
	void SendSetName(RakNet::RakPeerInterface* rakPeer, const RakNet::RakString& oldName, const RakNet::RakString& newName);
	
	void HandleGeneralChatMessage(RakNet::RakPeerInterface* rakPeer, RakNet::Packet* packet);
	void HandleGeneralChatMessageServer(RakNet::RakPeerInterface* rakPeer, RakNet::Packet* packet);
	
	void HandleGetChannels(RakNet::RakPeerInterface* rakPeer, RakNet::Packet* packet);
	void SendChannelList(RakNet::RakPeerInterface* rakPeer, RakNet::Packet* packet, std::vector<ChannelInfo*> channels);
	
	void HandleChangeChannels(RakNet::RakPeerInterface* rakPeer, RakNet::Packet* packet);
	void SendChangeChannelsResponse(RakNet::RakPeerInterface* rakPeer, RakNet::Packet* packet, ChangeChannelResponse response, ChannelInfo newChannelInfo);
}
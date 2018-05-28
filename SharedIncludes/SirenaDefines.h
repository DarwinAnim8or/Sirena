#pragma once
#include "stdafx.h"

enum SirenaMessages 
{
	ID_SIRENA_VERSION_CONFIRM = ID_USER_PACKET_ENUM + 1,
	ID_SIRENA_VERSION_ACCEPTED,
	ID_SIRENA_DISCONNECTION_NOTIFICATION,
	ID_SIRENA_MOTD,
	ID_SIRENA_SET_NAME,
	ID_SIRENA_GENERAL_CHAT_MESSAGE,
	ID_SIRENA_GET_CHANNELS,
	ID_SIRENA_CHANNEL_LIST,
	ID_SIRENA_CHANGE_CHANNELS,
	ID_SIRENA_CHANGE_CHANNELS_RESPONSE
};

struct VersionInfo
{
	unsigned char major = 0;
	unsigned char minor = 1;
};

enum class SirenaDisconnectionNotififiers
{
	KICKED,
	BANNED,
	VERSION_MISMATCH
};

struct ServerConfig
{
	RakNet::RakString serverName{};
	unsigned int serverPort{};
	RakNet::RakString motd{};
	RakNet::RakString serverToken{};
	unsigned int maxUsers{};
};

struct ChannelInfo
{
	RakNet::RakString channelName{"Unnamed Channel"};
	RakNet::RakString channelDesc{"This channel has no description."};
	bool isDefault{};
	unsigned char minRank{};
};

struct Rank
{
	unsigned char ID{};
	RakNet::RakString name{};
	bool isDefault{};
	unsigned int nameColor{};
};

struct User
{
	RakNet::RakString token{};
	RakNet::RakString name{};
	unsigned char rank{};
};

enum class ChangeChannelResponse
{
	UNKNOWN_ERROR,
	OK,
	NOT_ALLOWED,
	DOES_NOT_EXIST
};

enum class SirenaCommands
{
	//General:
	NICK,
	WHIPSER,
	DISCONNECT, //Executed on the client as well.
	STATS, //Executed on the client.
	AFK_TOGGLE,

	//Channels:
	WHO,
	GET_CHANNELS,
	CHANGE_CHANNEL,

	//User management:
	KICK,
	BAN,
	MUTE,

	//Roles:
	SET_ROLE,
	GET_ROLES
};

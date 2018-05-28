#pragma once
#include <unordered_map>
#include "SirenaDefines.h"

class UserPool
{
public:
	UserPool();
	~UserPool();

	void Insert(const RakNet::SystemAddress& address, User* user);
	void Remove(const RakNet::SystemAddress& address);
	User* Find(const RakNet::SystemAddress& address);

private:
	std::unordered_map<unsigned int, User*> m_Users;
};
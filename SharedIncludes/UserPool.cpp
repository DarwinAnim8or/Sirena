#include "stdafx.h"
#include "UserPool.h"

UserPool::UserPool()
{
}

UserPool::~UserPool()
{
	for (std::pair<unsigned int, User*> u : m_Users)
		delete u.second;
}

void UserPool::Insert(const RakNet::SystemAddress & address, User * user)
{
	m_Users.insert(std::pair<unsigned int, User*>(address.GetPort(), user));
}

void UserPool::Remove(const RakNet::SystemAddress & address)
{
	m_Users.erase(address.GetPort());
}

User * UserPool::Find(const RakNet::SystemAddress & address)
{
	return m_Users.find(address.GetPort())->second;
}

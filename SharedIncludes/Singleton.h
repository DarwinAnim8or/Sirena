#pragma once
template <typename T>
class Singleton
{
public:
	static T instance;
};

template<typename T>
T Singleton<T>::instance;
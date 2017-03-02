#pragma once

class CNetworkComponent;

class CNetworkComponentManager
{
public:
	CNetworkComponentManager();
	~CNetworkComponentManager();

	CNetworkComponent* CreateAndRegisterComponent(const unsigned aID);

private:

	std::map<unsigned, CNetworkComponent*> myComponents;
};


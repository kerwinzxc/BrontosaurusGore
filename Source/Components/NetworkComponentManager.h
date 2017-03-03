#pragma once

class CNetworkComponent;

class CNetworkComponentManager
{
public:
	static void Create();
	static void Destroy();
	static CNetworkComponentManager* GetInstance();

	CNetworkComponent* CreateAndRegisterComponent(const unsigned aID);
	CNetworkComponent* GetComponent(const unsigned aID);

private:
	CNetworkComponentManager();
	~CNetworkComponentManager();

	static CNetworkComponentManager* ourInstance;

	std::map<unsigned, CNetworkComponent*> myComponents;
};


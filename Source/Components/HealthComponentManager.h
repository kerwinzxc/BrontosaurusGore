#pragma once
#include <unordered_map>

class CHealthComponent;

class CHealthComponentManager
{
public:
	static void Create();
	static void Destroy();
	static CHealthComponentManager* GetInstance();

	CHealthComponent* CreateAndRegisterComponent(unsigned int aID);

	void TakeDamage(const unsigned int aID, const unsigned int aDamageTaken);

private:
	CHealthComponentManager();
	~CHealthComponentManager();

	static CHealthComponentManager* ourInstance;

	std::map<unsigned int, CHealthComponent*> myHealthComponents;

};


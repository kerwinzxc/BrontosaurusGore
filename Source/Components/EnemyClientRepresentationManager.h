#pragma once
#include "EnemyClientRepresentation.h"
class CWeaponSystemManager;
class CEnemyClientRepresentationManager
{
public:
	static void Create();
	static void Destroy();
	static CEnemyClientRepresentationManager& GetInstance();
	inline static bool CheckIfCreated();
	CEnemyClientRepresentation& CreateAndRegister();
	CEnemyClientRepresentation& GetRepresentation(unsigned short aId);
	void Update(const CU::Time& aDeltaTime);
	void Init(CWeaponSystemManager* aWeaponSystemManagerPointer);
protected:
	std::map<unsigned short, CEnemyClientRepresentation*> myRepresentations;
	
	static CEnemyClientRepresentationManager* ourInstance;
	CEnemyClientRepresentationManager();
	~CEnemyClientRepresentationManager();
};

inline bool CEnemyClientRepresentationManager::CheckIfCreated()
{
	return ourInstance != nullptr;
}

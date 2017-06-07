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
	bool CheckIfRepresentationExists(unsigned short aId);
	CEnemyClientRepresentation& GetRepresentation(unsigned short aId);
	void Update(const CU::Time& aDeltaTime);
	void Init(CWeaponSystemManager* aWeaponSystemManagerPointer);
	void CleanUpRepresentations();

protected:
	std::map<unsigned short, CEnemyClientRepresentation*> myRepresentations;
	
	static CEnemyClientRepresentationManager* ourInstance;
	static unsigned short ourID;

	CEnemyClientRepresentationManager();
	~CEnemyClientRepresentationManager();
};

inline bool CEnemyClientRepresentationManager::CheckIfCreated()
{
	return ourInstance != nullptr;
}

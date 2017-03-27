#pragma once
#include "EnemyClientRepresentation.h"
class CWeaponSystemManager;
class CEnemyClientRepresentationManager
{
public:
	static void Create();
	static void Destroy();
	static CEnemyClientRepresentationManager& GetInstance();
	CEnemyClientRepresentation& CreateAndRegister(unsigned int anId);
	CEnemyClientRepresentation& GetRepresentation(unsigned int aId);
	void Update(const CU::Time& aDeltaTime);
	void Init(CWeaponSystemManager* aWeaponSystemManagerPointer);
protected:
	std::map<unsigned int, CEnemyClientRepresentation*> myRepresentations;
	
	static CEnemyClientRepresentationManager* ourInstance;
	CEnemyClientRepresentationManager();
	~CEnemyClientRepresentationManager();
};


#pragma once
#include "../Components/EnemyBlueprint.h"
#include "../Components/EnemyTypes.h"

class CImpController;
class CRevenantController;
class CPinkyController;
class CEnemyComponentManager;
class CEnemy;
class CGameObjectManager;
class CWeaponSystemManager;
class CColliderComponentManager;

class CEnemyFactory
{
public:
	static void Create(CEnemyComponentManager& aEnemyManager, CGameObjectManager& aGameObjectManger, CWeaponSystemManager& aWeaponSystemManager, CColliderComponentManager& aColliderManager);
	static void Destroy();
	static CEnemyFactory* GetInstance();

	void LoadBluePrints(const std::string& alevel);

	CEnemy* CreateEnemy(const eEnemyTypes& aType, const CU::Vector3f& aPosition);
	CEnemy* CreateImp(const SImpBlueprint* aData,const CU::Vector3f& aPosition);
	CEnemy* CreateRevenant(const SRevenantBlueprint* aData);
	CEnemy* CreatePinky(const SPinkyBlueprint* aData); // skapar ett lill finger
	CEnemy* CreateRepesention(const short aHealthValue, const eEnemyTypes aType, const CU::Vector3f& aPosition, bool aShouldReset);
	

private:
	CEnemyFactory(CEnemyComponentManager& aEnemyManager, CGameObjectManager& aGameObjectManger, CWeaponSystemManager& aWeaponSystemManager, CColliderComponentManager& aColliderManager);
	~CEnemyFactory();

	SImpBlueprint myImpBluePrint;
	SRevenantBlueprint myRevenantBluePrint;
	SPinkyBlueprint myPinkyBluePrint;

	CEnemyComponentManager& myEnemyManager;
	CGameObjectManager& myGameObjectManager;
	CWeaponSystemManager& myWeaponSystemManager;
	CColliderComponentManager& myColliderManager;

	static CEnemyFactory* ourInstance;
};


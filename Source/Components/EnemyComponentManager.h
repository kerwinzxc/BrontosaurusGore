#pragma once
#include "EnemyBlueprint.h"
#include "EnemyTypes.h"
class CScene;
class CComponent;

class CEnemy;
class CWeaponSystemManager;
class CColliderComponentManager;

class CEnemyComponentManager
{
public:
	//struct EnemyBlueprint
	//{
	//	unsigned int health;
	//	float speed;
	//	float detactionRange;
	//	float startAttackRange;
	//	float stopAttackRange;
	//};
	explicit CEnemyComponentManager();

	void Update(const float aDeltaTime);

	void InitWeaponSystem(CEnemy* aEnemy, CWeaponSystemManager* aWeaponSystemManager);
	CEnemy* CreateComponent(const SEnemyBlueprint* anEnemyBlueprint, eEnemyTypes aType);
	CComponent* CreateComponentAbstract(const SEnemyBlueprint* anEnemyBlueprint, eEnemyTypes aType);
	void DeleteComponent(CEnemy* anEnemy);
	void Init(CWeaponSystemManager* aWeaponSystemComponentManagerPointer);
	~CEnemyComponentManager();
	inline bool GetIsInited();
protected:
	CU::GrowingArray<CEnemy*> myEnemies;

	bool myIsInited;
};

inline bool CEnemyComponentManager::GetIsInited()
{
	return myIsInited;
}
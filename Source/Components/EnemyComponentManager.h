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

	CEnemy* CreateComponent(const SEnemyBlueprint* anEnemyBlueprint, unsigned int anId, eEnemyTypes aType);
	CComponent* CreateComponentAbstract(const SEnemyBlueprint* anEnemyBlueprint, unsigned int anId, eEnemyTypes aType);
	void DeleteComponent(CEnemy* anEnemy);
	void Init(CWeaponSystemManager* aWeaponSystemComponentManagerPointer);
	~CEnemyComponentManager();
protected:
	CU::GrowingArray<CEnemy*> myEnemies;

};


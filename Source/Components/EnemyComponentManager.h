#pragma once
#include "EnemyBlueprint.h"
class CScene;
class CComponent;

class CEnemy;


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

	CEnemy* CreateComponent(const SEnemyBlueprint& anEnemyBlueprint, unsigned int anId);
	CComponent* CreateComponentAbstract(const SEnemyBlueprint& anEnemyBlueprint, unsigned int anId);
	void DeleteComponent(CEnemy* anEnemy);

	~CEnemyComponentManager();
protected:
	CU::GrowingArray<CEnemy*> myEnemies;

};


#pragma once

class CScene;
class CComponent;

namespace Component
{
	class CEnemy;
}

class CEnemyComponentManager
{
public:
	struct EnemyBlueprint
	{
		unsigned int health;
		float speed;
		float detactionRange;
		float startAttackRange;
		float stopAttackRange;
	};
	explicit CEnemyComponentManager(CScene& aScene);

	void Update(const CU::Time& aDeltaTime);

	Component::CEnemy* CreateComponent(const EnemyBlueprint& anEnemyBlueprint);
	CComponent* CreateComponentAbstract(const EnemyBlueprint& anEnemyBlueprint);
	void DeleteComponent(Component::CEnemy* anEnemy);

	~CEnemyComponentManager();
protected:
	CU::GrowingArray<Component::CEnemy*> myEnemies;
	CScene& myScene;
};


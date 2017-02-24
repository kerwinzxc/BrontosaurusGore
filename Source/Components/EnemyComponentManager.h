#pragma once
class CScene;

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
	static CEnemyComponentManager& GetInstance();
	explicit CEnemyComponentManager(CScene& aScene);

	void Update(const CU::Time& aDeltaTime);

	Component::CEnemy* CreateComponent(const EnemyBlueprint& anEnemyBlueprint);
	void DeleteComponent(Component::CEnemy* anEnemy);

	~CEnemyComponentManager();
protected:
	static CEnemyComponentManager* ourInstance;

	CU::GrowingArray<Component::CEnemy*> myEnemies;
	CScene& myScene;
};


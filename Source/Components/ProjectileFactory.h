#pragma once
class CGameObjectManager;
class CModelComponentManager;
class ProjectileComponent;
struct ProjectileData;
class ProjectileComponentManager;
class ProjectileFactory
{
public:
	ProjectileFactory(ProjectileComponentManager* aProjectileComponentManager);
	~ProjectileFactory();
	inline static ProjectileFactory& GetInstance();
	void ShootProjectile(ProjectileData* someData, CU::Vector3f aDirection, CU::Vector3f aSpawnPosition);
	void Update(float aDeltaTime);
	void Init(CGameObjectManager* aGameObjectManager, CModelComponentManager* aModelComponentManagerPointer);
	void CreateProjectile();
private:
	CU::GrowingArray<ProjectileComponent*> myActiveProjectiles;
	CU::GrowingArray<ProjectileComponent*> myPassiveProjectiles;
	static ProjectileFactory* ourInstance;
	CGameObjectManager* myGameObjectManagerPointer;
	CModelComponentManager* myModelComponentManagerPointer;
	ProjectileComponentManager* myProjectileComponentManager;
};

inline ProjectileFactory& ProjectileFactory::GetInstance()
{
	return *ourInstance;
}
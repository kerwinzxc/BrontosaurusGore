#pragma once
class CGameObjectManager;
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
	void Init(CGameObjectManager* aGameObjectManager);
	void CreateProjectile();
private:
	CU::GrowingArray<ProjectileComponent*> myActiveProjectiles;
	CU::GrowingArray<ProjectileComponent*> myPassiveProjectiles;
	static ProjectileFactory* ourInstance;
	CGameObjectManager* myGameObjectManagerPointer;
	ProjectileComponentManager* myProjectileComponentManager;
};

inline ProjectileFactory& ProjectileFactory::GetInstance()
{
	return *ourInstance;
}
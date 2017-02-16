#pragma once
class CGameObjectManager;
class ProjectileComponent;
struct ProjectileData;
class ProjectileFactory
{
public:
	ProjectileFactory();
	~ProjectileFactory();
	inline static ProjectileFactory& GetInstance();
	void ShootProjectile(ProjectileData* someData, CU::Vector3f aDirection, CU::Vector3f aSpawnPosition);
	void Update(float aDeltaTime);
	void Init(CGameObjectManager* aGameObjectManager);
	void CreateProjectile();
private:
	static ProjectileFactory* ourInstance;
	CU::GrowingArray<ProjectileComponent*> myActiveProjectiles;
	CU::GrowingArray<ProjectileComponent*> myPassiveProjectiles;
	CGameObjectManager* myGameObjectManagerPointer;
};

inline ProjectileFactory& ProjectileFactory::GetInstance()
{
	return *ourInstance;
}
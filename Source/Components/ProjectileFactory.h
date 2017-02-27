#pragma once
class CGameObjectManager;
class CModelComponentManager;
class CProjectileComponent;
struct SProjectileData;
class CProjectileComponentManager;
class CProjectileFactory
{
public:
	CProjectileFactory(CProjectileComponentManager* aProjectileComponentManager);
	~CProjectileFactory();
	inline static CProjectileFactory& GetInstance();
	void ShootProjectile(SProjectileData* someData, CU::Vector3f aDirection, CU::Vector3f aSpawnPosition);
	void Update(float aDeltaTime);
	void Init(CGameObjectManager* aGameObjectManager, CModelComponentManager* aModelComponentManagerPointer);
	void CreateProjectile();
private:
	CU::GrowingArray<CProjectileComponent*> myActiveProjectiles;
	CU::GrowingArray<CProjectileComponent*> myPassiveProjectiles;
	static CProjectileFactory* ourInstance;
	CGameObjectManager* myGameObjectManagerPointer;
	CModelComponentManager* myModelComponentManagerPointer;
	CProjectileComponentManager* myProjectileComponentManager;
};

inline CProjectileFactory& CProjectileFactory::GetInstance()
{
	return *ourInstance;
}
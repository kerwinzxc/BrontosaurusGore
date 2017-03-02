#pragma once
class CGameObjectManager;
class CModelComponentManager;
class CProjectileComponent;
struct SProjectileData;
class CProjectileComponentManager;
struct SProjectileBufferData;
class CProjectileFactory
{
public:
	CProjectileFactory(CProjectileComponentManager* aProjectileComponentManager);
	~CProjectileFactory();
	inline static CProjectileFactory* GetInstance();
	void Init(CGameObjectManager* aGameObjectManager, CModelComponentManager* aModelComponentManagerPointer);
	void Update(float aDeltaTime);
	void ShootProjectile(SProjectileData* someData, CU::Vector3f aDirection, CU::Vector3f aSpawnPosition);
	void ShootProjectile(SProjectileData* someData, CU::Vector3f aDirection, CU::Vector3f aSpawnPosition, unsigned int aIndex);
	void CreateNewProjectileBuffer(std::string aProjctileName, unsigned int aSuggestedProjectileBufferSize);
private:
	void CreateProjectile(unsigned int aIndex);
private:
	CU::GrowingArray<SProjectileBufferData*> myActiveProjectiles;
	CU::GrowingArray<SProjectileBufferData*> myPassiveProjectiles;
	static CProjectileFactory* ourInstance;
	CGameObjectManager* myGameObjectManagerPointer;
	CModelComponentManager* myModelComponentManagerPointer;
	CProjectileComponentManager* myProjectileComponentManager;
};

inline CProjectileFactory* CProjectileFactory::GetInstance()
{
	return ourInstance;
}
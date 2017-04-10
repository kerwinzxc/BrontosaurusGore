#pragma once
#include <thread>
class CSpawnerComponent;
enum class eEnemyTypes;

class CSpawnerManager
{
public:
	CSpawnerManager();
	~CSpawnerManager();

	CSpawnerComponent* CreateSpawnerComponent(const CU::GrowingArray<unsigned short>& aWaves, const eEnemyTypes& eEnemyType, std::thread::id aID);
	void Update(const float aDeltaTIme);
private:
	CU::GrowingArray<CSpawnerComponent*> mySpawners;
};


#pragma once
#include <thread>

class CTriggerArenaComponent;


class CArenaTriggerComponentManager
{
public:
	CArenaTriggerComponentManager(const std::thread::id aThreadID);
	~CArenaTriggerComponentManager();

	void Update();

	CTriggerArenaComponent* CreateAndRegisterComponent(const unsigned char aNumberOfWaves, const short aKeyId, const short aArenaID);

private:

	CU::GrowingArray<CTriggerArenaComponent*> myComponents;
	std::thread::id myThreadID;
};


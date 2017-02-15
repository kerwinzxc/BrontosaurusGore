#pragma once
#include "AudioSourceComponent.h"
#include <unordered_map>

class CAudioSourceComponentManager
{
public:
	static CAudioSourceComponentManager& GetInstance();
	static void Create();
	static void Destroy();

	CAudioSourceComponent* CreateComponent();

	void Update();
	void RemoveAll();
	void Remove(CAudioSourceComponent * aComponentToRemove);
private:
	CAudioSourceComponentManager();
	~CAudioSourceComponentManager();
	
	static CAudioSourceComponentManager* myInstance;

	std::unordered_map < Audio::GameObjectID, unsigned int> myIds;
	CU::GrowingArray<CAudioSourceComponent*, unsigned int> myComponents;
		
};


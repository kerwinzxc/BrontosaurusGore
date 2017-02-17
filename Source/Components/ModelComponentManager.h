#pragma once
#include "ModelComponent.h"
#include "../BrontosaurusEngine/ModelShapes.h"

#define MODELCOMP_MGR CModelComponentManager::GetInstance()

class CScene;

class CModelComponentManager
{
public:
	static CModelComponentManager& GetInstance();

	CModelComponentManager(CScene& aScene);
	~CModelComponentManager();

	CModelComponent* CreateComponent(const char* aModelPath);
	//CModelComponent* CreateComponent(SShape aShape);

	void DeleteModelComponent(CModelComponent* aComponent);

private:
	static CModelComponentManager* ourInstance;

	CU::GrowingArray<CModelComponent*> myModels;
	CScene& myScene;
};


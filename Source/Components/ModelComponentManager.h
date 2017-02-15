#pragma once
#include "ModelComponent.h"
#include "../BrontosaurusEngine/ModelShapes.h"

#define MODELCOMP_MGR CModelComponentManager::GetInstance()

class CScene;

class CModelComponentManager
{
public:
	static CModelComponentManager& GetInstance();
	static void Create();
	static void Destroy();

	CModelComponent* CreateComponent(const char* aModelPath);
	CModelComponent* CreateComponent(SShape aShape);
	void SetScene(CScene* aScene);

	void DeleteModelComponent(CModelComponent* aComponent);

private:
	CModelComponentManager();
	~CModelComponentManager();

private:
	static CModelComponentManager* ourInstance;

	CU::GrowingArray<CModelComponent*> myModels;
	CScene* myScene;
};


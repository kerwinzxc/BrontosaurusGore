#include "stdafx.h"
#include "ModelComponentManager.h"
#include "ComponentManager.h"
#include "../BrontosaurusEngine/Engine.h"
#include "../BrontosaurusEngine/ModelManager.h"
#include "../BrontosaurusEngine/ModelInstance.h"
#include "../BrontosaurusEngine/Scene.h"

CModelComponentManager* CModelComponentManager::ourInstance = nullptr;


void CModelComponentManager::Destroy()
{
	assert(ourInstance != nullptr && "Model component manager not created (is NULL)");
	SAFE_DELETE(ourInstance);
}

CModelComponentManager::CModelComponentManager()
{
	myModels.Init(16);
}


CModelComponentManager::~CModelComponentManager()
{
	//myModels.DeleteAll();
}

void CModelComponentManager::SetScene(CScene * aScene)
{
	myScene = aScene;
}

void CModelComponentManager::DeleteModelComponent(CModelComponent* aComponent)
{
	CModelInstance* const modelInstance = aComponent->GetAndReleaseModel();
	myScene->DeleteModelInstance(modelInstance);
	myModels.DeleteCyclic(aComponent);
}

CModelComponentManager& CModelComponentManager::GetInstance()
{
	assert(ourInstance != nullptr && "Model component manager not created (is NULL)");
	return *ourInstance;
}

void CModelComponentManager::Create()
{
	assert(ourInstance == nullptr && "Model component manager already created (is NULL)");
	ourInstance = new CModelComponentManager();
}

CModelComponent* CModelComponentManager::CreateComponent(const char * aModelPath)
{
	CModelInstance* instance = new CModelInstance(aModelPath);
	InstanceID modelID = myScene->AddModelInstance(instance);
	CModelComponent* modelComp = new CModelComponent(*instance);
	myModels.Add(modelComp);
	CComponentManager::GetInstance().RegisterComponent(myModels.GetLast());
	return myModels.GetLast();
}

CModelComponent* CModelComponentManager::CreateComponent(SShape aShape)
{
	CModelInstance* instance = new CModelInstance(aShape);

	//InstanceID modelID = myScene->AddDebugObjectInstance(instance);
			//later add the shape to the scene (and make sure it's not col info)
	CModelComponent* modelComp = new CModelComponent(*instance, true);

	myModels.Add(modelComp);
	return myModels.GetLast();
}

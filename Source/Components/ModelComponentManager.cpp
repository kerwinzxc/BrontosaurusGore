#include "stdafx.h"
#include "ModelComponentManager.h"
#include "ComponentManager.h"
#include "../BrontosaurusEngine/Engine.h"
#include "../BrontosaurusEngine/ModelManager.h"
#include "../BrontosaurusEngine/ModelInstance.h"
#include "../BrontosaurusEngine/Scene.h"

CModelComponentManager* CModelComponentManager::ourInstance = nullptr;

CModelComponentManager::CModelComponentManager(CScene& aScene)
	: myModels(16u)
	, myScene(aScene)
{
	assert(ourInstance == nullptr);
	ourInstance = this;
}

CModelComponentManager::~CModelComponentManager()
{
	assert(ourInstance == this);
	ourInstance = nullptr;
}

void CModelComponentManager::DeleteModelComponent(CModelComponent* aComponent)
{
	myScene.DeleteModelInstance(&aComponent->GetModelInstance());

	CComponentManager::GetInstance().DeleteComponent(aComponent->GetId());
	myModels.RemoveCyclic(aComponent);
}

CModelComponentManager& CModelComponentManager::GetInstance()
{
	assert(ourInstance != nullptr && "Model component manager not created (is NULL)");
	return *ourInstance;
}

CModelComponent* CModelComponentManager::CreateComponent(const std::string& aModelPath)
{
	CModelInstance* instance = new CModelInstance(aModelPath);
	/*InstanceID modelID = */(void)myScene.AddModelInstance(instance);
	CModelComponent* modelComp = new CModelComponent(*instance); // send in the id and a reference to the scene instead?
	myModels.Add(modelComp);
	
	CComponentManager::GetInstance().RegisterComponent(myModels.GetLast());

	return myModels.GetLast();
}

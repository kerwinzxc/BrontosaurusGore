#include "stdafx.h"
#include "PointLightComponentManager.h"
#include "PointLightComponent.h"
#include "ComponentManager.h"
#include "Scene.h"

CPointLightComponentManager* CPointLightComponentManager::ourInstance = nullptr;

void CPointLightComponentManager::Destroy()
{
	SAFE_DELETE(ourInstance);
}

void CPointLightComponentManager::Create(CScene& aScene)
{
	if (ourInstance == nullptr)
	{
		ourInstance = new CPointLightComponentManager(aScene);
	}
	else
	{
		assert(false);
	}
}

CPointLightComponentManager& CPointLightComponentManager::GetInstance()
{
	return *ourInstance;
}

PointLightComponent* CPointLightComponentManager::CreateAndRegisterComponent()
{
	PointLightComponent* pointLight = new PointLightComponent(myScene);
	CComponentManager::GetInstance().RegisterComponent(pointLight);
	myComponents.Add(pointLight);
	return pointLight;
}

void CPointLightComponentManager::Update(const CU::Time aDeltaTime)
{
	for (PointLightComponent* component : myComponents)
	{
		component->Update(aDeltaTime);
	}
}

CPointLightComponentManager::CPointLightComponentManager(CScene& aScene)
	: myScene(aScene)
	, myComponents(10)
{
}

CPointLightComponentManager::~CPointLightComponentManager()
{
}

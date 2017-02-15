#include "stdafx.h"
#include "CameraManager.h"
#include "ComponentManager.h"

CCameraComponentManager* CCameraComponentManager::myInstance = nullptr;

CCameraComponentManager::CCameraComponentManager()
{
	myCameraComponents.Init(1);
}


CCameraComponentManager::~CCameraComponentManager()
{
	//for (unsigned int i = 0; i < myCameraComponents.Size(); i++)
	//{
	//	CComponentManager::GetInstance().RemoveComponent(myCameraComponents[i]->GetId());
	//	SAFE_DELETE(myCameraComponents[i]);
	//}

	myCameraComponents.RemoveAll();
}

void CCameraComponentManager::Create()
{
	assert(myInstance == nullptr && "Camera manager already created");
	myInstance = new CCameraComponentManager;
}

void CCameraComponentManager::Destroy()
{
	assert(myInstance != nullptr && "Camera manager not created (is NULL)");
	SAFE_DELETE(myInstance);
}

CCameraComponentManager& CCameraComponentManager::GetInstance()
{
	assert(myInstance != nullptr && "Camera manager not created (is NULL)");
	return *myInstance;
}

CCameraComponent* CCameraComponentManager::CreateCameraComponent()
{
	CCameraComponent* camera = new CCameraComponent();

	CComponentManager::GetInstance().RegisterComponent(camera);

	myCameraComponents.Add(camera);

	return  camera;
}

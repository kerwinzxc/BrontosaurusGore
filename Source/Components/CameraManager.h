#pragma once
#include "CameraComponent.h"
#include "Component.h"
#include "../CommonUtilities/Camera.h"

class CCameraComponentManager
{
public:
	static void Create();
	static void Destroy();
	static CCameraComponentManager& GetInstance();

	CCameraComponent* CreateCameraComponent();

private: 
	CCameraComponentManager();
	~CCameraComponentManager();

	CU::GrowingArray<CCameraComponent*> myCameraComponents;

	static CCameraComponentManager* myInstance;
};


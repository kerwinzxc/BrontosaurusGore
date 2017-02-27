#include "stdafx.h"
#include "LoadCamera.h"
#include "CameraComponent.h"
//#include "../BrontosaurusEngine/Scene.h"

int LoadCamera(KLoader::SLoadedComponentData someData)
{
	GET_LOADMANAGER(loadManager);

	CU::Camera& playerCamera = loadManager.GetCurrentScene().GetCamera(CScene::eCameraType::ePlayerOneCamera);

	CCameraComponent* cameraComponent = new CCameraComponent();
	CComponentManager::GetInstance().RegisterComponent(cameraComponent);
	cameraComponent->SetCamera(playerCamera);

	return cameraComponent->GetId();
}

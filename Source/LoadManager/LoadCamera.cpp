#include "stdafx.h"
#include "LoadCamera.h"
#include "CameraComponent.h"

struct SEncapsulationBreaker
{
	static void SetCameraComponent(CPlayState& aPlayState, CCameraComponent* aCameraComponent)
	{
		aPlayState.myCameraComponent = aCameraComponent;
	}
};

int LoadCamera(KLoader::SLoadedComponentData /*someData*/)
{
	GET_LOADMANAGER(loadManager);

	CU::Camera& playerCamera = loadManager.GetCurrentScene().GetRenderCamera(CScene::eCameraType::ePlayerOneCamera).GetCamera();

	CCameraComponent* cameraComponent = new CCameraComponent();
	CComponentManager::GetInstance().RegisterComponent(cameraComponent);
	cameraComponent->SetCamera(playerCamera);

	SEncapsulationBreaker::SetCameraComponent(loadManager.GetCurrentPLaystate(), cameraComponent);

	return cameraComponent->GetId();
}

#include "stdafx.h"
#include "LoadCamera.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ComponentManager.h"
#include "CameraComponent.h"
#include "CameraManager.h"
#include "../BrontosaurusEngine/Scene.h"
#include "../CommonUtilities/matrix33.h"

int LoadCamera(KLoader::SLoadedComponentData someData)
{
	CCameraComponent* Camera = CCameraComponentManager::GetInstance().CreateCameraComponent();

	CU::Matrix44f cameraTransformation = LoadManager::GetInstance().GetCurrentScene()->GetCamera(CScene::eCameraType::ePlayerOneCamera).GetTransformation();
	CU::Matrix44f newRotation;


	// för att återgå till gamla ta bort Y rotationen och sätt så X rotationen delas med 4
	newRotation.Rotate(3.14159265f / 4, CU::Axees::Y);
	newRotation.Rotate(3.14159265f / 3.5f, CU::Axees::X);

	cameraTransformation.SetRotation(newRotation);

	//cameraTransformation.Rotate(3.14159265f / 8, CU::Axees::Y);

	cameraTransformation.SetPosition(CU::Vector3f(0.0f, 0.0f, 0.0f));
	cameraTransformation.Move(CU::Vector3f(0.0f, 0.0f, -1400.0f));

	LoadManager::GetInstance().GetCurrentScene()->GetCamera(CScene::eCameraType::ePlayerOneCamera).SetTransformation(cameraTransformation);

	Camera->SetCamera(LoadManager::GetInstance().GetCurrentScene()->GetCamera(CScene::eCameraType::ePlayerOneCamera));

	Camera->InitOffsetPosition();

	return Camera->GetId();
}

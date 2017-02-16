#include "stdafx.h"
#include "PlayState.h"

#include <Engine.h>
#include <CommonUtilities.h>
#include <Lights.h>
#include <TimerManager.h>
#include <Scene.h>

#include "Skybox.h"
#include "../Audio/AudioInterface.h"

#include "../PostMaster/PostMaster.h"
#include "../PostMaster/Message.h"
#include "../PostMaster/Event.h"


//Managers

#include "Components/GameObject.h"
#include "Components/GameObjectManager.h"
#include "Components/ComponentManager.h"
//#include "../GUI/GUIManager.h"

#include "LoadManager/LoadManager.h"
#include "KevinLoader/KevinLoader.h"
#include "CameraComponent.h"


CPlayState::CPlayState(StateStack& aStateStack, const int aLevelIndex)
	: State(aStateStack, eInputMessengerType::ePlayState)
	, myLevelIndex(aLevelIndex)
	, myScene(nullptr)
{
	myIsLoaded = false;
}

CPlayState::~CPlayState()
{
	SAFE_DELETE(myScene);
	SAFE_DELETE(myGameObjectManager);
	CComponentManager::DestroyInstance();
}

void CPlayState::Load()
{
	CU::TimerManager timerMgr;
	CU::TimerHandle handle = timerMgr.CreateTimer();
	timerMgr.StartTimer(handle);

	srand(static_cast<unsigned int>(time(nullptr)));

	LoadManagerGuard loadManagerGuard;

	CComponentManager::CreateInstance();
	myScene = new CScene();
	myGameObjectManager = new CGameObjectManager();

	LoadManager::GetInstance().SetCurrentPlayState(this);
	LoadManager::GetInstance().SetCurrentScene(myScene);

	Lights::SDirectionalLight dirLight;
	dirLight.color = { .25f, .25f, .25f, 1.0f };
	dirLight.direction = { -1.0f, -1.0f, 1.0f, 1.0f };
	myScene->AddDirectionalLight(dirLight);


	myScene->AddCamera(CScene::eCameraType::ePlayerOneCamera);
	CU::Camera& playerCamera = myScene->GetCamera(CScene::eCameraType::ePlayerOneCamera);
	playerCamera.Init(90, WINDOW_SIZE_F.x, WINDOW_SIZE_F.y, 1.f, 75000.0f);

	//CCameraComponent* cameraComponent = CCameraComponentManager::GetInstance().CreateCameraComponent();
	CCameraComponent* cameraComponent = new CCameraComponent();
	CComponentManager::GetInstance().RegisterComponent(cameraComponent);
	cameraComponent->SetCamera(playerCamera);
	CGameObject* playerObject = myGameObjectManager->CreateGameObject();
	CGameObject* cameraObject = myGameObjectManager->CreateGameObject();
	cameraObject->GetLocalTransform().SetPosition(0.f, 200.f, 0.f);
	cameraObject->AddComponent(cameraComponent);
	playerObject->AddComponent(cameraObject);

	myGameObjectManager->SendObjectsDoneMessage();

	myScene->SetSkybox("default_cubemap.dds");
	
	//KLoader::CKevinLoader &loader = KLoader::CKevinLoader::GetInstance();
	//
	//const KLoader::eError loadError = loader.LoadFile(levelPath);
	//if (loadError != KLoader::eError::NO_LOADER_ERROR)
	//{
	//	DL_ASSERT("Loading Failed");
	//}

	myIsLoaded = true;

	// Get time to load the level:
	timerMgr.UpdateTimers();
	float time = timerMgr.GetTimer(handle).GetLifeTime().GetMilliseconds();
	GAMEPLAY_LOG("Game Inited in %f ms", time);

}

void CPlayState::Init()
{
}

eStateStatus CPlayState::Update(const CU::Time& aDeltaTime)
{
	myScene->GetCamera(CScene::eCameraType::ePlayerOneCamera).TranslateForward(aDeltaTime.GetSeconds() * 1000.f);
	myScene->GetCamera(CScene::eCameraType::ePlayerOneCamera).Jaw(aDeltaTime.GetSeconds() * 3.1415f / 2.f);

	myScene->Update(aDeltaTime);
	return myStatus;
}

void CPlayState::Render()
{
	myScene->Render();
}

void CPlayState::OnEnter(const bool /*aLetThroughRender*/)
{
}

void CPlayState::OnExit(const bool /*aLetThroughRender*/)
{
}

void CPlayState::Pause()
{
}

eMessageReturn CPlayState::Recieve(const Message& aMessage)
{
	return aMessage.myEvent.DoEvent(this);
}

CGameObjectManager* CPlayState::GetGameObjectManager()
{
	return myGameObjectManager;
}

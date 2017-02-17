#include "stdafx.h"
#include "PlayState.h"

#include <Engine.h>
#include <CommonUtilities.h>
#include <StopWatch.h>
#include <EInputReturn.h>
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
#include "InputComponent.h"
#include <chrono>
#include <thread>
#include "MovementComponent.h"
#include "ModelComponentManager.h"


CPlayState::CPlayState(StateStack& aStateStack, const int aLevelIndex)
	: State(aStateStack, eInputMessengerType::ePlayState, 1)
	, myLevelIndex(aLevelIndex)
	, myGameObjectManager(nullptr)
	, myScene(nullptr)
	, myMovementComponent(nullptr)
	, myIsLoaded(false)
{
}

CPlayState::~CPlayState()
{
	SAFE_DELETE(myGameObjectManager);
	SAFE_DELETE(myScene);

	CComponentManager::DestroyInstance();
}

void CPlayState::Load()
{
	CU::TimerManager timerMgr;
	CU::TimerHandle handle = timerMgr.CreateTimer();
	timerMgr.StartTimer(handle);
	CU::CStopWatch loadPlaystateTimer;

	srand(static_cast<unsigned int>(time(nullptr)));

	LoadManagerGuard loadManagerGuard;

	CComponentManager::CreateInstance();

	myScene = new CScene();
	myGameObjectManager = new CGameObjectManager();
	
	CModelComponentManager::Create();
	CModelComponentManager::GetInstance().SetScene(myScene);

	LoadManager::GetInstance().SetCurrentPlayState(this);
	LoadManager::GetInstance().SetCurrentScene(myScene);

	Lights::SDirectionalLight dirLight;
	dirLight.color = { .25f, .25f, .25f, 1.0f };
	dirLight.direction = { -1.0f, -1.0f, 1.0f, 1.0f };
	myScene->AddDirectionalLight(dirLight);


	myScene->AddCamera(CScene::eCameraType::ePlayerOneCamera);
	CU::Camera& playerCamera = myScene->GetCamera(CScene::eCameraType::ePlayerOneCamera);
	playerCamera.Init(90, WINDOW_SIZE_F.x, WINDOW_SIZE_F.y, 1.f, 75000.0f);
	
	//create player:
	{
		CCameraComponent* cameraComponent = new CCameraComponent();
		CComponentManager::GetInstance().RegisterComponent(cameraComponent);
		cameraComponent->SetCamera(playerCamera);
		CGameObject* playerObject = myGameObjectManager->CreateGameObject();
		CGameObject* cameraObject = myGameObjectManager->CreateGameObject();
		cameraObject->GetLocalTransform().SetPosition(0.f, 1.8f, 0.f);
		cameraObject->AddComponent(cameraComponent);
		playerObject->AddComponent(cameraObject);

		CInputComponent* inputComponent = new CInputComponent();
		playerObject->AddComponent(inputComponent);

		myMovementComponent = new CMovementComponent();
		playerObject->AddComponent(myMovementComponent);
	}
	

	myGameObjectManager->SendObjectsDoneMessage();

	myScene->SetSkybox("default_cubemap.dds");
	
	KLoader::CKevinLoader &loader = KLoader::CKevinLoader::GetInstance();

	const KLoader::eError loadError = loader.LoadFile("Json/HubWorld/LevelData.json");
	if (loadError != KLoader::eError::NO_LOADER_ERROR)
	{
		DL_MESSAGE_BOX("Loading Failed");
	}

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
	myMovementComponent->Update(aDeltaTime);
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

CU::eInputReturn CPlayState::RecieveInput(const CU::SInputMessage& aInputMessage)
{
	CU::CInputMessenger::RecieveInput(aInputMessage);
	return CU::eInputReturn::eKeepSecret;
}

CGameObjectManager* CPlayState::GetGameObjectManager()
{
	return myGameObjectManager;
}

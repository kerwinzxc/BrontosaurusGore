#include "stdafx.h"
#include "PlayState.h"

#include <Engine.h>
#include <CommonUtilities.h>
#include <Lights.h>
#include <TimerManager.h>
#include <Renderer.h>
#include <Scene.h>
#include <time.h>
#include <fstream>

#include "Skybox.h"
#include "../Audio/AudioInterface.h"

#include "../PostMaster/PostMaster.h"
#include "../PostMaster/Message.h"
#include "../PostMaster/PopCurrentState.h"



//Managers

#include "Components/GameObject.h"
#include "Components/GameObjectManager.h"

#include "Components/CameraManager.h"
#include "Components/ModelComponentManager.h"

#include "Components/AudioSourceComponentManager.h"
#include "Components/ParticleEmitterComponentManager.h"
#include "Components/ComponentManager.h"
#include "Components/CollisionComponentManager.h"
#include "Components/PointLightComponentManager.h"
#include "Components/PointLightComponent.h"

#include "../GUI/GUIManager.h"

#include "LoadManager/LoadManager.h"
//--------
#include "BrontosaurusEngine/LineDrawer.h"
#include "BrontosaurusEngine/TextInstance.h"

#include "../LuaWrapper/SSlua/SSlua.h"




//Kanske Inte ska vara här?
#include "../BrontosaurusEngine/Console.h"
//Temp Includes
#include "KevinLoader/KevinLoader.h"


CPlayState::CPlayState(StateStack& aStateStack, const int aLevelIndex, const bool aShouldReturnToLevelSelect)
	: State(aStateStack)
	, myLevelIndex(aLevelIndex)
	, myShouldReturnToLevelSelect(aShouldReturnToLevelSelect)
	, myScene(nullptr)
{
	myIsLoaded = false;
}

CPlayState::~CPlayState()
{
	SAFE_DELETE(myScene);

	CModelComponentManager::Destroy();
	CAudioSourceComponentManager::Destroy();
	CParticleEmitterComponentManager::Destroy();
	CCameraComponentManager::Destroy();
	CPointLightComponentManager::Destroy();
	CComponentManager::DestroyInstance();
}

void CPlayState::Load()
{
	//start taking the time for loading level
	if (myShouldReturnToLevelSelect == true)
	{
		myLevelIndex++;
	}
	CU::TimerManager timerMgr;
	CU::TimerHandle handle = timerMgr.CreateTimer();
	timerMgr.StartTimer(handle);
	srand(static_cast<unsigned int>(time(NULL)));

	CreateManagersAndFactories();
	LoadManagerGuard loadManagerGuard;


	MODELCOMP_MGR.SetScene(myScene);
	LoadManager::GetInstance().SetCurrentPlayState(this);
	LoadManager::GetInstance().SetCurrentScene(myScene);


	Lights::SDirectionalLight dirLight;
	dirLight.color = { .25f, .25f, .25f, 1.0f };
	dirLight.direction = { -1.0f, -1.0f, 1.0f, 1.0f };
	myScene->AddDirectionalLight(dirLight);

	CONSOLE->GetLuaFunctions();

	myScene->AddCamera(CScene::eCameraType::ePlayerOneCamera);
	CU::Camera& playerCamera = myScene->GetCamera(CScene::eCameraType::ePlayerOneCamera);
	playerCamera.Init(90, WINDOW_SIZE_F.x, WINDOW_SIZE_F.y, 1.f, 75000.0f);

	CCameraComponent* cameraComponent = CCameraComponentManager::GetInstance().CreateCameraComponent();
	cameraComponent->SetCamera(myScene->GetCamera(CScene::eCameraType::ePlayerOneCamera));


	//Loading
	//CU::CJsonValue levelsFile;

	//std::string errorString = levelsFile.Parse("Json/LevelList.json");
	//if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());

	//CU::CJsonValue levelsArray = levelsFile.at("levels");

	//myLevelIndex = 0;// levelsArray.Size() - 1;

	//std::string levelPath = "Json/Levels/";
	//levelPath += levelsArray[myLevelIndex].GetString();
	//levelPath += "/LevelData.json";

	//std::string questPath = "Json/Quests/";
	//questPath += levelsArray[myLevelIndex].GetString();
	//questPath += ".json";

	//std::string navmeshPath = "Models/Navmesh/";
	//navmeshPath += levelsArray[myLevelIndex].GetString();
	//navmeshPath -= ".json";
	//navmeshPath += "_navmesh.obj";
	////NAVMESH
	//{
	//	std::ifstream infile(navmeshPath);
	//	if (infile.good())
	//	{
	//		myNavmesh.LoadFromFile(navmeshPath.c_str());
	//		PollingStation::Navmesh = &myNavmesh;
	//	}
	//	else
	//	{
	//		PollingStation::Navmesh = nullptr;
	//	}
	//}

	// Cubemap & Skybox
	/*std::string cubemapPath = "Models/Cubemaps/";
	cubemapPath += levelsArray[myLevelIndex].GetString();
	cubemapPath -= ".json";
	cubemapPath += "_cubemap.dds";
	{
		std::ifstream infile(cubemapPath);
		if (infile.good())
		{
			myScene->SetSkybox(cubemapPath.c_str());
		}
		else
		{
		}
	}*/

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
	CParticleEmitterComponentManager::GetInstance().UpdateEmitters(aDeltaTime);


	CPointLightComponentManager::GetInstance().Update(aDeltaTime);

	return myStatus;
}

void CPlayState::Render()
{
	myScene->Render();
	SChangeStatesMessage msg;

//#ifndef _RETAIL_BUILD
//	if (myShuldRenderNavmesh == true)
//	{
//		msg.myBlendState = eBlendState::eAlphaBlend;
//		msg.myDepthStencilState = eDepthStencilState::eDisableDepth;
//		msg.myRasterizerState = eRasterizerState::eWireFrame;
//		msg.mySamplerState = eSamplerState::eClamp;
//
//		RENDERER.AddRenderMessage(new SChangeStatesMessage(msg));
//		myNavmesh.Render();
//	}
//#endif

}

void CPlayState::OnEnter(const bool aLetThroughRender)
{
}

void CPlayState::OnExit(const bool aLetThroughRender)
{
}

void CPlayState::Pause()
{
}


void CPlayState::CheckReturnToLevelSelect()
{
	if (myShouldReturnToLevelSelect == true)
	{
		PostMaster::GetInstance().SendLetter(Message(eMessageType::eStateStackMessage, PopCurrentState()));
	}
}

eMessageReturn CPlayState::Recieve(const Message& aMessage)
{
	return aMessage.myEvent.DoEvent(this);
}


void CPlayState::CreateManagersAndFactories()
{
	myScene = new CScene();
	CAudioSourceComponentManager::Create();
	CComponentManager::CreateInstance();
	CModelComponentManager::Create();
	CParticleEmitterComponentManager::Create();
	CParticleEmitterComponentManager::GetInstance().SetScene(myScene);
	CPointLightComponentManager::Create(*myScene);
	CCameraComponentManager::Create();
}

CCollisionComponentManager* CPlayState::GetCollisionManager()
{
	throw std::logic_error("The method or operation is not implemented.");
	return nullptr;

}

CGameObjectManager* CPlayState::GetObjectManager()
{
	throw std::logic_error("The method or operation is not implemented.");
	return nullptr;
}

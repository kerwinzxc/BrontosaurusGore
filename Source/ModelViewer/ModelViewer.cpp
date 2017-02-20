#include "stdafx.h"
#include "ModelViewer.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../PostMaster/DroppedFile.h"

const InstanceID NoModel = 1337u;

CModelViewer::CModelViewer()
	: myScene(nullptr)
	, myCurrentModel(NoModel)
{
}

CModelViewer::~CModelViewer()
{
	//PostMaster::GetInstance().UnSubscribe(this, eMessageType::eDroppedFile);
	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
	SAFE_DELETE(myScene);
}

void CModelViewer::Init(const char aStartModelPath[])
{
	myScene = new CScene();

	CU::Vector2f windowSize(WINDOW_SIZE);
	myScene->AddCamera(CScene::eCameraType::ePlayerOneCamera);
	CU::Camera& camera = myScene->GetCamera(CScene::eCameraType::ePlayerOneCamera);
	camera.Init(60, windowSize.x, windowSize.y, 1.f, 75000.0f);
	camera.SetPosition({ 0.f, 75.f , -200.f });

	myScene->SetSkybox("skybox.dds");




	Lights::SDirectionalLight directionalLight;
	directionalLight.color.Set(1.f, 1.f, 1.f, 1.f);
	directionalLight.direction.Set(0.f, 0.f, 1.f, 1.f);
	myScene->AddDirectionalLight(directionalLight);

	if (aStartModelPath)
	{
		LoadModel(aStartModelPath);
	}

	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eDroppedFile);
}

void CModelViewer::Update(const CU::Time& aDeltaTime)
{
	myScene->Update(aDeltaTime);
	if (myCurrentModel != NoModel)
	{
		CU::Matrix44f transform = myScene->GetModelAt(myCurrentModel)->GetTransformation();
		transform.Rotate(2.5f * aDeltaTime.GetSeconds(), CU::Axees::Y);
		myScene->GetModelAt(myCurrentModel)->SetTransformation(transform);
	}
}

void CModelViewer::Render()
{
	myScene->Render();
}

void CModelViewer::LoadModel(const std::string& aModelPath)
{
	if (myCurrentModel != NoModel)
	{
		myScene->DeleteModelInstance(myCurrentModel);
	}
	
	CModelInstance* newModelInstance = new CModelInstance(aModelPath.c_str());
	myCurrentModel = myScene->AddModelInstance(newModelInstance);
}

eMessageReturn CModelViewer::DoEvent(const DroppedFile& aDroppedFile)
{
	const std::string& filePath = aDroppedFile.GetFilePath();

	if(filePath.rfind(".fbx") != std::string::npos)
	{
		LoadModel(filePath);
	}
	else
	{
		DL_PRINT_WARNING("File is not fbx: %s", filePath.c_str());
	}

	return eMessageReturn::eContinue;
}

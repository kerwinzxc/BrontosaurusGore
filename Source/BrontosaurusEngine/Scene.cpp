﻿#include "stdafx.h"
#include "Scene.h"
#include "ModelInstance.h"
#include "Engine.h"
#include "ModelManager.h"
#include "PointLightInstance.h"
#include "Camera.h"
#include "Renderer.h"
#include "Skybox.h"
#include "ParticleEmitterInstance.h"
#include "FireEmitterInstance.h"

#define Intify(A_ENUM_CLASS) static_cast<int>(A_ENUM_CLASS)
#define SHADOWBUFFER_DIM 2048

CScene::CScene()
{
	myModels.Init(64);
	myPointLights.Init(32);
	myParticleEmitters.Init(8);
	myFireEmitters.Init(8);
	mySkybox = nullptr;

	myCubemap = nullptr;

	myShadowCamera.InitOrthographic(1024, 1024, 1024, 50.f, SHADOWBUFFER_DIM, SHADOWBUFFER_DIM);
	myShadowCamera.SetIsShadowCamera(true);
}

CScene::~CScene()
{
	if (mySkybox && mySkybox->DecRef() <= 0)
	{
		SAFE_DELETE(mySkybox);
	}
	myModels.DeleteAll();
	myParticleEmitters.DeleteAll();
	//myDebugObjects.DeleteAll();
}

void CScene::Update(const CU::Time aDeltaTime)
{
	for (CModelInstance* model : myModels)
	{
		model->Update(aDeltaTime);
	}
	for (CParticleEmitterInstance* particle : myParticleEmitters)
	{
		if (particle == nullptr) continue;
		particle->Update(aDeltaTime);
	}
}

void CScene::Render()
{
	if(myCubemap)
		myCubemap->SetShaderResource();

	SSetCameraMessage cameraMsg;
	cameraMsg.myCamera = myCameras[Intify(eCameraType::ePlayerOneCamera)];
	RENDERER.AddRenderMessage(new SSetCameraMessage(cameraMsg));

	CU::Vector3f shadowCamDirection = { myDirectionalLight.direction.x, myDirectionalLight.direction.y, myDirectionalLight.direction.z };
	CU::Vector3f shadowCameraPosition = myCameras[Intify(eCameraType::ePlayerOneCamera)].GetPosition() + (-shadowCamDirection * 500);
	myShadowCamera.GetCamera().SetPosition(shadowCameraPosition);
	myShadowCamera.GetCamera().LookAt(myCameras[Intify(eCameraType::ePlayerOneCamera)].GetPosition());

	SChangeStatesMessage statemsg;

	if (mySkybox)
	{
		statemsg.myBlendState = eBlendState::eNoBlend;
		statemsg.myRasterizerState = eRasterizerState::eNoCulling;
		statemsg.myDepthStencilState = eDepthStencilState::eDisableDepth;
		statemsg.mySamplerState = eSamplerState::eWrap;

		RENDERER.AddRenderMessage(new SChangeStatesMessage(statemsg));

		SRenderSkyboxMessage* msg = new SRenderSkyboxMessage();
		mySkybox->AddRef();
		msg->mySkybox = mySkybox;
		RENDERER.AddRenderMessage(msg);
	}

	statemsg.myRasterizerState = eRasterizerState::eDefault;
	statemsg.myDepthStencilState = eDepthStencilState::eDefault;
	statemsg.myBlendState = eBlendState::eNoBlend;
	statemsg.mySamplerState = eSamplerState::eClamp;

	RENDERER.AddRenderMessage(new SChangeStatesMessage(statemsg));
	myShadowCamera.AddRenderMessage(new SChangeStatesMessage(statemsg));

	CU::VectorOnStack<CPointLightInstance, 8> culledPointlights;


	for (unsigned int i = 0; i < myPointLights.Size(); ++i)
	{
		if (myPointLights[i].GetIsActive() == false)
		{
			continue;
		}

		CU::AABB lightBB;
		lightBB.myCenterPos = myPointLights[i].GetPosition();
		lightBB.myRadius = myPointLights[i].GetRange();

		if (myShadowCamera.GetCamera().IsInside(lightBB) == false)
		{
			continue;
		}
		culledPointlights.SafeAdd(myPointLights[i]); 

		SRenderPointLight pointlightMessage;
		pointlightMessage.pointLight = myPointLights[i].GetData();
		RENDERER.AddRenderMessage(new SRenderPointLight(pointlightMessage));
	}


	for (unsigned int i = 0; i < myModels.Size(); ++i)
	{
		if (myModels[i] == nullptr || myModels[i]->ShouldRender() == false)
		{
			continue;
		}
		/*if (myShadowCamera.GetCamera().IsInside(myModels[i]->GetModelBoundingBox()) == false)
		{
			continue;
		}*/

		myModels[i]->Render(myShadowCamera);
	}


	myShadowCamera.Render();
	SSetShadowBuffer *shadowMSG = new SSetShadowBuffer();
	shadowMSG->myCameraProjection = myShadowCamera.GetCamera().GetProjection();
	shadowMSG->myCameraTransformation = myShadowCamera.GetCamera().GetInverse();
	shadowMSG->myShadowBuffer = myShadowCamera.GetRenderPackage();
	RENDERER.AddRenderMessage(shadowMSG);

	for (unsigned int i = 0; i < myModels.Size(); ++i)
	{
		if (myModels[i] == nullptr || myModels[i]->ShouldRender() == false)
		{
			continue;
		}
		myModels[i]->Render();
	}

	SRenderDirectionalLight light;
	light.directionalLight = myDirectionalLight;
	RENDERER.AddRenderMessage(new SRenderDirectionalLight(light));



	SChangeStatesMessage* changeStateMessage = new SChangeStatesMessage();
	changeStateMessage->myBlendState = eBlendState::eAlphaBlend;
	changeStateMessage->myDepthStencilState = eDepthStencilState::eReadOnly; //don't know what to do here
	changeStateMessage->myRasterizerState = eRasterizerState::eNoCullingClockwise;
	changeStateMessage->mySamplerState = eSamplerState::eClamp0Wrap1;
	RENDERER.AddRenderMessage(changeStateMessage);

	for (CFireEmitterInstance& fireEmitter : myFireEmitters)
	{
		fireEmitter.GetTransformation().LookAt(myCameras[Intify(eCameraType::ePlayerOneCamera)].GetPosition());
		fireEmitter.Render();
	}

	statemsg.myBlendState = eBlendState::eAlphaBlend;
	statemsg.myRasterizerState = eRasterizerState::eDefault;
	statemsg.myDepthStencilState = eDepthStencilState::eReadOnly;
	statemsg.mySamplerState = eSamplerState::eClamp;

	RENDERER.AddRenderMessage(new SChangeStatesMessage(statemsg));

	for (unsigned int i = 0; i < myParticleEmitters.Size(); ++i)
	{
		if (myParticleEmitters[i] == nullptr || myParticleEmitters[i]->IsVisible() == false)
		{
			continue;
		}

		myParticleEmitters[i]->Render(GetCamera(eCameraType::ePlayerOneCamera));
	}


	// DRAW SHADOWBUFFER

	//SRenderToIntermediate * interMSG = new SRenderToIntermediate();
	//interMSG->myRect = { 0.0f, 0.0f, 0.25f, 0.25f };
	//interMSG->useDepthResource = true;
	//interMSG->myRenderPackage = myShadowCamera.GetRenderPackage();
	//RENDERER.AddRenderMessage(interMSG);

}

InstanceID CScene::AddModelInstance(CModelInstance* aModelInstance)
{
	InstanceID id = 0;

	if (myFreeModels.Size() < 1)
	{
		id = myModels.Size();
		myModels.Add(aModelInstance);
		return id;
	}

	id = myFreeModels.Pop();
	myModels[id] = aModelInstance;
	
	
	return id;
}

InstanceID CScene::AddDirectionalLight(const Lights::SDirectionalLight & aDirectionalLight)
{
	myDirectionalLight = aDirectionalLight;
	return 0;
}

InstanceID CScene::AddPointLightInstance(const CPointLightInstance& aPointLight)
{
	InstanceID id = 0;

	if (myFreePointlights.Size() < 1)
	{
		id = myPointLights.Size();
		myPointLights.Add(aPointLight);
		return id;
	}

	id = myFreePointlights.Pop();
	myPointLights[id] = aPointLight;
	myPointLights[id].SetActive(true);
	return id;
}

InstanceID CScene::AddParticleEmitterInstance(CParticleEmitterInstance* aParticleEmitterInstance)
{
	if (myFreeParticleEmitters.Size() < 1)
	{
		InstanceID temp = myParticleEmitters.Size();
		myParticleEmitters.Add(aParticleEmitterInstance);
		return temp;
	}

	InstanceID tempId = myFreeParticleEmitters.Pop();
	myParticleEmitters[tempId] = aParticleEmitterInstance;

	return  tempId;
}

InstanceID CScene::AddFireEmitters(const CFireEmitterInstance& aFireEmitter)
{
	InstanceID id = myFireEmitters.Size();
	myFireEmitters.Add(aFireEmitter);
	return id;
}

void CScene::AddCamera(const eCameraType aCameraType)
{
	myCameras[static_cast<int>(aCameraType)] = CU::Camera(); //TODO: maybe not have add
}

void CScene::SetSkybox(const char* aPath)
{
	if (mySkybox != nullptr)
	{
		delete mySkybox;
	}

	mySkybox = new CSkybox();
	mySkybox->Init(aPath);
}
void CScene::SetSkybox(ID3D11ShaderResourceView* aSRV)
{
	if (mySkybox != nullptr)
	{
		if (mySkybox->DecRef() <= 0)
		{
			SAFE_DELETE(mySkybox);
		}
	}

	mySkybox = new CSkybox();
	mySkybox->Init(aSRV);
}

void CScene::SetCubemap(const char* aPath)
{
	SAFE_DELETE(myCubemap);
	myCubemap = new CCubemap(aPath);
}

CModelInstance* CScene::GetModelAt(const InstanceID aModelID)
{
	if (myModels.HasIndex(aModelID))
	{
		return myModels[aModelID];
	}

	return nullptr;
}

CFireEmitterInstance& CScene::GetFireEmitter(const InstanceID aFireEmitterID)
{
	if (aFireEmitterID >= myFireEmitters.Size() || aFireEmitterID < 0)
	{
		static CFireEmitterInstance nullFireEmitter;
		return nullFireEmitter;
	}

	return myFireEmitters[aFireEmitterID];
}

CU::Camera& CScene::GetCamera(const eCameraType aCameraType)
{
	return myCameras[static_cast<int>(aCameraType)];
}

CParticleEmitterInstance* CScene::GetParticleEmitterInstance(const InstanceID aParticleEmitterID)
{
	return (myParticleEmitters.HasIndex(aParticleEmitterID)) ? myParticleEmitters[aParticleEmitterID] : nullptr;
}

CPointLightInstance * CScene::GetPointLightInstance(const InstanceID aID)
{
	return (myPointLights.HasIndex(aID)) ? &myPointLights[aID] : nullptr;
}

void CScene::DeleteModelInstance(CModelInstance* anInstance)
{
	InstanceID currentId;

	if (myModels.Find(anInstance, currentId) == false)
	{
		return;
	}

	DeleteModelInstance(currentId);
}

void CScene::DeleteModelInstance(const InstanceID& anId)
{
	//if thread lock here
	CModelInstance* currentModel = myModels[anId];
	myModels[anId] = nullptr;
	myFreeModels.Push(anId);
	//if thread unlock here

	delete currentModel;
}

void CScene::DeleteParticleEmitterInstance(const InstanceID anID)
{
	//if thread lock here
	
	//static std::mutex lockHere;
	//lockHere.lock();
	
	CParticleEmitterInstance* emitter = myParticleEmitters[anID];
	myParticleEmitters[anID] = nullptr;
	myFreeParticleEmitters.Push(anID);

	//lockHere.unlock();
	
	//if thread unlock here
	
	
	delete emitter;
	//emitter->Destroy();
}

void CScene::RemovePointLightInstance(const InstanceID anID)
{
	myPointLights[anID].SetActive(false);
	myFreePointlights.Push(anID);

}

void CScene::GenerateCubemap()
{
	//for now, not shure if we even need this

	return;


	//SChangeStatesMessage statemsg;
	//SSetCameraMessage cameraMsg;
	//CU::VectorOnStack<CPointLightInstance, 8> culledPointlights;

	//static float piOver2 = 3.1415926f / 2.0f;

	//CU::Matrix33f rotations[6];
	//rotations[1] = CU::Matrix33f::CreateRotateAroundY(-piOver2);
	//rotations[0] = CU::Matrix33f::CreateRotateAroundY(piOver2);
	//rotations[3] = CU::Matrix33f::CreateRotateAroundX(piOver2);
	//rotations[2] = CU::Matrix33f::CreateRotateAroundX(-piOver2);
	//rotations[4] = CU::Matrix33f::Identity;
	//rotations[5] = CU::Matrix33f::CreateRotateAroundY(PI);

	//CU::Camera camera;
	//camera.Init(90.f, width, height, 1.0f, 100.f);

	//for (int i = 0; i < 6; ++i)
	//{
	//	myCubemap->ActivateForRender(i);
	//	cameraMsg.myCamera = camera;
	//	cameraMsg.myCamera.SetTransformation(rotations[i]);
	//	RENDERER.AddRenderMessage(new SSetCameraMessage(cameraMsg));

	//	if (mySkybox)
	//	{
	//		statemsg.myBlendState = eBlendState::eNoBlend;
	//		statemsg.myRasterizerState = eRasterizerState::eNoCulling;
	//		statemsg.myDepthStencilState = eDepthStencilState::eDisableDepth;
	//		statemsg.mySamplerState = eSamplerState::eWrap;
	//		RENDERER.AddRenderMessage(new SChangeStatesMessage(statemsg));
	//		SRenderSkyboxMessage* msg = new SRenderSkyboxMessage();
	//		mySkybox->AddRef();
	//		msg->mySkybox = mySkybox;
	//		RENDERER.AddRenderMessage(msg);
	//		statemsg.myRasterizerState = eRasterizerState::eDefault;
	//		statemsg.myDepthStencilState = eDepthStencilState::eDefault;
	//		statemsg.myBlendState = eBlendState::eNoBlend;
	//		statemsg.mySamplerState = eSamplerState::eClamp;
	//		RENDERER.AddRenderMessage(new SChangeStatesMessage(statemsg));
	//	}
	//	
	//	
	//	// mebe not neededo? - said Greedo
	//	CU::Vector3f lightDir = myDirectionalLight.direction;
	//	myDirectionalLight.direction = CU::Vector3f::Zero;
	//	//
	//	for (unsigned int j = 0; j < myModels.Size(); ++j)
	//	{
	//		if (myModels[j] == nullptr || myModels[j]->ShouldRender() == false)
	//		{
	//			continue;
	//		}
	//		myModels[j]->Render(&myDirectionalLight, culledPointlights);
	//	}
	//	myDirectionalLight.direction = lightDir;
	//}

	//RENDERER.AddRenderMessage(new SActivateRenderToMessage());
	//myCubemap->SetShaderResource();
}

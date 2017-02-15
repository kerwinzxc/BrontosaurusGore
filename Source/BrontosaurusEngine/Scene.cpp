#include "stdafx.h"
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

namespace 
{
	float Distance2(const CU::Vector3f& p1, const CU::Vector3f& p2)
	{
		return ((p1.x - p2.x) * (p1.x - p2.x) + (p1.y - p2.y) * (p1.y - p2.y) + (p1.z - p2.z) * (p1.z - p2.z));
	}

	int partition(CU::GrowingArray<CModelInstance*, InstanceID>& A, int p, int q);
	
	void quickSort(CU::GrowingArray<CModelInstance*, InstanceID>& A, int p, int q)
	{
		int r;
		if (p < q)
		{
			r = partition(A, p, q);
			quickSort(A, p, r);
			quickSort(A, r + 1, q);
		}
	}


	int partition(CU::GrowingArray<CModelInstance*, InstanceID>& A, int p, int q)
	{
		//TODO: FIX CAMERA :)
		float x = 0.f;// Distance2(A[p]->GetTransformation().GetPosition(), CAMERA->GetPosition());
		int i = p;
		int j;

		CModelInstance* temp = nullptr;

		for (j = p + 1; j < q; j++)
		{
			//TODO: FIX CAMERA :) maybe send in as argument
			float y = 0;// Distance2(A[j]->GetTransformation().GetPosition(), CAMERA->GetPosition());
			if (y > x)
			{
				i = i + 1;
				temp = A[i];
				A[i] = A[j];
				A[j] = temp;
			}

		}

		temp = A[i];
		A[i] = A[p];
		A[p] = temp;

		return i;
	}

}



CScene::CScene()
{
	myModels.Init(64);
	myPointLights.Init(32);
	myParticleEmitters.Init(8);
	myFireEmitters.Init(8);
	mySkybox = nullptr;

	myShadowCamera.InitOrthographic(7000, 3000, 7000.f, 50.f, 4098, 4098);
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
	SSetCameraMessage cameraMsg;
	cameraMsg.myCamera = myCameras[Intify(eCameraType::ePlayerOneCamera)];
	RENDERER.AddRenderMessage(new SSetCameraMessage(cameraMsg));


	//EDVIN POLLAR PLAYERPOS HÄR, DETTA BORDE GÖRAS FINARE :C
	CU::Vector3f shadowCamDirection = { myDirectionalLight.direction.x, myDirectionalLight.direction.y, myDirectionalLight.direction.z };
	CU::Vector3f shadowCameraPosition = myCameras[Intify(eCameraType::ePlayerOneCamera)].GetPosition() + (-shadowCamDirection * 2000);
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
	}


	for (unsigned int i = 0; i < myModels.Size(); ++i)
	{
		if (myModels[i] == nullptr || myModels[i]->ShouldRender() == false)
		{
			continue;
		}
		if (myShadowCamera.GetCamera().IsInside(myModels[i]->GetModelBoundingBox()) == false)
		{
			continue;
		}

		myModels[i]->Render(nullptr, culledPointlights, myShadowCamera);
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

		if (myCameras[Intify(eCameraType::ePlayerOneCamera)].IsInside(myModels[i]->GetModelBoundingBox()) == false)
		{
			continue;
		}

		myModels[i]->Render(&myDirectionalLight, culledPointlights);
	}

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

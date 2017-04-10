#include "stdafx.h"
#include "ParticleEmitterComponentManager.h"
#include "ComponentManager.h"
#include "ParticleEmitterComponent.h"

#include "..\CommonUtilities\DL_Debug.h"
#include "..\CommonUtilities\CUTime.h"

#include "..\BrontosaurusEngine\Scene.h"
#include "..\BrontosaurusEngine\ParticleEmitterInstance.h"
#include "ParticleEmitterManager.h"


CParticleEmitterComponentManager* CParticleEmitterComponentManager::myInstance = nullptr;

void CParticleEmitterComponentManager::Create()
{
	if (myInstance != nullptr)
	{
		DL_ASSERT("ParticleEmitter Component already created");
	}

	if (myInstance == nullptr)
		myInstance = new CParticleEmitterComponentManager();
}

CParticleEmitterComponentManager & CParticleEmitterComponentManager::GetInstance()
{
	if (myInstance == nullptr)
	{
		DL_ASSERT("ParticleEmitter Component Instance not Created");
	}
	return *myInstance;
}

void CParticleEmitterComponentManager::Destroy()
{
	delete myInstance;
	myInstance = nullptr;
}

CParticleEmitterComponent * CParticleEmitterComponentManager::CreateComponent(const std::string& aSystemId)
{
	Particles::ParticleEmitterID emitter = CParticleEmitterManager::GetInstance().GetParticleEmitterId(aSystemId);
	myComponents.Add(new CParticleEmitterComponent(emitter));
	CComponentManager::GetInstance().RegisterComponent(myComponents.GetLast());
	//emitter->SetInstandeID(id);
	return myComponents.GetLast();
}

void CParticleEmitterComponentManager::RemoveAll()
{
	//ehhe?
	for (unsigned int i = myComponents.Size(); i > 0; --i)
	{
		Remove(myComponents[i - 1]);
	}
}

void CParticleEmitterComponentManager::Remove(CParticleEmitterComponent * aComponentToRemove)
{
	//CComponentManager::GetInstance().RemoveComponent(aComponentToRemove->GetId());
	//myScene->DeleteParticleEmitterInstance(aComponentToRemove->GetEmitter()->GetInstanceID());
	//myComponents.DeleteCyclic(aComponentToRemove);
}

void CParticleEmitterComponentManager::SetScene(CScene * aScene)
{
	myScene = aScene;
}

void CParticleEmitterComponentManager::UpdateEmitters(const CU::Time & aDeltaTime)
{
	for (unsigned int i = 0; i < myComponents.Size(); ++i)
	{
		myComponents[i]->Update(aDeltaTime);
	}
}

CParticleEmitterComponentManager::CParticleEmitterComponentManager()
{
	myComponents.Init(8);
	myScene = nullptr;
}

CParticleEmitterComponentManager::~CParticleEmitterComponentManager()
{
	RemoveAll();
	myScene = nullptr;
}

#include "stdafx.h"
#include "ParticleEmitterManager.h"
#include "EmitterData.h"
#include "ParticleEmitter.h"
#include "picojson.h"
#include "../Particles/ParticleLibrary.h"
#include "ParticleEmitterInstance.h"
#include "Work.h"
#include "ThreadPool.h"


CParticleEmitterManager* CParticleEmitterManager::ourInstance = nullptr;

CParticleEmitterManager::CParticleEmitterManager(): myIsActive(true), myThreadFinished(false)
{
	myParticleEmitterComp.Init(16);
	myInstances.Init(10);
}


CParticleEmitterManager::~CParticleEmitterManager()
{
	myIsActive = false;
	while (myThreadFinished == false)
	{
		
	}
}

CParticleEmitterManager& CParticleEmitterManager::GetInstance()
{
	if(ourInstance == nullptr)
	{
		DL_ASSERT("Particle emitter manager has not been created");
	}

	return *ourInstance;;
}

void CParticleEmitterManager::Create()
{
	ourInstance = new CParticleEmitterManager;
}

void CParticleEmitterManager::Destroy()
{
	SAFE_DELETE(ourInstance);
}

void CParticleEmitterManager::RemoveParticleEmitter(const Particles::ParticleEmitterID aID)
{
	
	//if (myParticleEmitterComp[aID].myRefCount <= 0)
	//{
	//	myParticleEmitters[aID].Destroy();

	//	myParticleEmitterComp[aID].myID = -1;
	//	myParticleEmitterComp[aID].myNumOfParticles = 0;
	//	myParticleEmitterComp[aID].myTexturePath = "";
	//	myParticleEmitterComp[aID].myRefCount = 0;

	//	myFreeParticleEmitterIDs.Push(aID);
	//}
}

void CParticleEmitterManager::LoadParticleLibrary(const std::string& aLibraryPath)
{
	std::map<std::basic_string<char>, Particles::CParticleLibrary*>::iterator it = myLibraries.find(aLibraryPath);

	if(it == myLibraries.end())
	{
		myLibraries[aLibraryPath] = new Particles::CParticleLibrary;
		myLibraries[aLibraryPath]->Load(aLibraryPath);
	}
}

Particles::ParticleEmitterID CParticleEmitterManager::GetEmitterInstance(Particles::ParticleEmitterID aId)
{
	CU::GrowingArray<CParticleEmitterInstance*>& freeInstances = GetFreeInstances(aId);
	CParticleEmitterInstance* result = nullptr;

	if (freeInstances.Size() == 0)
	{
		CParticleEmitter* emitter = GetEmitter(aId);
		if(emitter != nullptr && emitter->CanInstansiate() == true)
		{
			result = new CParticleEmitterInstance(aId);	
		}
		else
		{
			return Particles::CParticleLibrary::FoundNone;
		}
	}
	else
	{
		result = freeInstances[freeInstances.Size() - 1];
		freeInstances.RemoveCyclicAtIndex(freeInstances.Size() -1);
	}
	result->AddRef();
	myInstances.Add(result);
	return result->GetInstanceID();
}

Particles::ParticleEmitterID CParticleEmitterManager::GetEmitterInstance(const std::string& aId)
{
	return GetParticleEmitterId(aId);
}

Particles::ParticleEmitterID CParticleEmitterManager::GetParticleEmitterId(const std::string& aSystemId)
{
	std::map<std::basic_string<char>, Particles::CParticleLibrary*>::iterator it;
	for(it = myLibraries.begin(); it != myLibraries.end();++it)
	{
		Particles::ParticleEmitterID id = it->second->GetSystem(aSystemId);
		if (id != it->second->FoundNone)
		{
			return GetEmitterInstance(id);
		}
	}

	DL_ASSERT("Could not find particle emitter \"%s\"", aSystemId.c_str());
	return GetEmitterInstance(Particles::CParticleLibrary::FoundNone);
}

void CParticleEmitterManager::Release(Particles::ParticleEmitterID anInstanceId)
{
	for(int i = 0; i < myInstances.Size();++i)
	{
		if(myInstances[i]->GetInstanceID() == anInstanceId)
		{
			CU::GrowingArray<CParticleEmitterInstance*>& freeInstances = GetFreeInstances(myInstances[i]->GetEmitterId());
			myInstances[i]->Release();
			break;
		}
	}
}

void CParticleEmitterManager::SetPosition(int aInstanceId, const CU::Vector3f& aPosition)
{
	for(int i = 0; i < myInstances.Size(); ++i)
	{
		if(myInstances[i]->GetInstanceID() == aInstanceId)
		{
			myInstances[i]->SetPosition(aPosition);
			break;
		}
	}
}

void CParticleEmitterManager::Activate(Particles::ParticleEmitterID aInstanceId)
{
	for (int i = 0; i < myInstances.Size(); ++i)
	{
		if (myInstances[i]->GetInstanceID() == aInstanceId)
		{
			myInstances[i]->Activate();
			break;
		}
	}
}

void CParticleEmitterManager::Deactivate(Particles::ParticleEmitterID aInstanceId)
{
	for (int i = 0; i < myInstances.Size(); ++i)
	{
		if (myInstances[i]->GetInstanceID() == aInstanceId)
		{
			myInstances[i]->Deactivate();
			break;
		}
	}
}

CParticleEmitter* CParticleEmitterManager::GetEmitter(Particles::ParticleEmitterID aParticleEmitter)
{
	std::map<std::basic_string<char>, Particles::CParticleLibrary*>::iterator it;
	for(it = myLibraries.begin(); it != myLibraries.end(); ++it)
	{
		CParticleEmitter* emitter = it->second->GetSystemP(aParticleEmitter);

		if(emitter != nullptr)
		{
			return emitter;
		}
	}

	return nullptr;
}

void CParticleEmitterManager::Update(const CU::Time aDeltaTime)
{
	for(int i = myInstances.Size() - 1; i >=0 ; --i)
	{
		if (myInstances[i]->ShouldKeep() == true)
		{
			CParticleEmitter* emitter = GetEmitter(myInstances[i]->GetEmitterId());
			if(emitter != nullptr)
			{
				CParticleEmitterInstance& instance = *myInstances[i];
				emitter->UpdateInstance(aDeltaTime, instance);
				int i = 0;
			}
		}
		else
		{
			ReleaseInternal(myInstances[i]->GetInstanceID());
		}
	}
}

void CParticleEmitterManager::Render(CRenderCamera& aRenderCamera)
{
	for(int i = 0; i < myInstances.Size();++i)
	{
		if(myInstances[i]->IsVisible() == true)
		{
			myInstances[i]->Render(aRenderCamera);
		}
	}
}

void CParticleEmitterManager::Run()
{
	std::function<void(void)> func = std::bind(&CParticleEmitterManager::InternalRun, this);
	CU::Work work(func, CU::ePriority::eHigh);
	work.SetName("Particle thread"); 

	std::function<bool(void)> condition = std::bind(&CParticleEmitterManager::IsActive, this);
	work.AddLoopCondition(condition); 
	std::function<void(void)> callback = std::bind(&CParticleEmitterManager::ThreadFinished, this);
	work.SetFinishedCallback(callback);
	myTimerManager.CreateTimer();
	myTimerManager.UpdateTimers();
	CU::ThreadPool::GetInstance()->AddWork(work);
}

bool CParticleEmitterManager::IsActive() const
{
	return myIsActive;
}

void CParticleEmitterManager::SetTransformation(int aInstanceId, const CU::Matrix44f& aMatrix44)
{
	for (int i = 0; i < myInstances.Size(); ++i)
	{
		if (myInstances[i]->GetInstanceID() == aInstanceId)
		{
			myInstances[i]->SetTransformation(aMatrix44);
			break;
		}
	}
}

void CParticleEmitterManager::ReleaseInternal(InstanceID aInstanceId)
{
	for (int i = 0; i < myInstances.Size(); ++i)
	{
		if (myInstances[i]->GetInstanceID() == aInstanceId)
		{
			CU::GrowingArray<CParticleEmitterInstance*>& freeInstances = GetFreeInstances(myInstances[i]->GetEmitterId());
			freeInstances.Add(myInstances[i]);
			myInstances.RemoveCyclicAtIndex(i);
			break;
		}
	}
}

void CParticleEmitterManager::ThreadFinished()
{
	myThreadFinished = true;
}

void CParticleEmitterManager::InternalRun()
{
	myTimerManager.UpdateTimers();
	Update(myTimerManager.GetTimer(0).GetDeltaTime());
}

CU::GrowingArray<CParticleEmitterInstance*>& CParticleEmitterManager::GetFreeInstances(Particles::ParticleEmitterID aId)
{
	std::map<int, CU::GrowingArray<CParticleEmitterInstance*>>::iterator it = myFreeInstances.find(aId);

	if (it == myFreeInstances.end())
	{
		myFreeInstances[aId].Init(5);
	}

	return myFreeInstances[aId];
}

bool CParticleEmitterManager::SParticleEmitterComparer::operator==(const SEmitterData & aEmitterData)
{
	return	static_cast<unsigned int>(aEmitterData.NumOfParticles) == myNumOfParticles &&
		aEmitterData.TexturePath == myTexturePath;
}

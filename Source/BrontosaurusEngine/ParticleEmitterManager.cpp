#include "stdafx.h"
#include "ParticleEmitterManager.h"
#include "EmitterData.h"
#include "ParticleEmitter.h"

CParticleEmitterManager::CParticleEmitterManager()
{
	myParticleEmitterComp.Init(16);
	myParticleEmitters.Init(16);
}


CParticleEmitterManager::~CParticleEmitterManager()
{
}

const CParticleEmitterManager::ParticleEmitterID CParticleEmitterManager::CreateParticleEmitter(const SEmitterData & aEmitterData)
{
	ParticleEmitterID id = EmitterExists(aEmitterData);
	
	if (id == -1)
	{
		SParticleEmitterComparer comp;
		comp.myNumOfParticles = aEmitterData.NumOfParticles;
		comp.myTexturePath = aEmitterData.TexturePath;
		
		if (myFreeParticleEmitterIDs.Size() == 0)
		{
			id = myParticleEmitters.Size();
			myParticleEmitters.Add(CParticleEmitter());
			myParticleEmitterComp.Add(SParticleEmitterComparer());

		}
		else
		{
			id = myFreeParticleEmitterIDs.Pop();
			myParticleEmitterComp[id] = comp;
		}
		comp.myID = id;
		myParticleEmitterComp[id] = comp;
		myParticleEmitters[id].Init(aEmitterData);
	}

	AddRef(id);
	return id;
}

CParticleEmitter * CParticleEmitterManager::GetParticleEmitter(const ParticleEmitterID aID)
{
	if (myParticleEmitterComp[aID].myID == -1)
	{
		return nullptr;
	}

	return &myParticleEmitters[aID];
}

void CParticleEmitterManager::RemoveParticleEmitter(const ParticleEmitterID aID)
{
	RemoveRef(aID);
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

int CParticleEmitterManager::EmitterExists(const SEmitterData & aEmitterData)
{
	for (unsigned int i = 0; i < myParticleEmitterComp.Size(); i++)
	{
		if (myParticleEmitterComp[i] == aEmitterData)
		{
			return myParticleEmitterComp[i].myID;
		}
	}

	return -1;
}

void CParticleEmitterManager::AddRef(const ParticleEmitterID aID)
{
	myParticleEmitterComp[aID].myRefCount++;
}

void CParticleEmitterManager::RemoveRef(const ParticleEmitterID aID)
{
	myParticleEmitterComp[aID].myRefCount--;
}

bool CParticleEmitterManager::SParticleEmitterComparer::operator==(const SEmitterData & aEmitterData)
{
	return	aEmitterData.NumOfParticles == myNumOfParticles &&
		aEmitterData.TexturePath == myTexturePath;
}
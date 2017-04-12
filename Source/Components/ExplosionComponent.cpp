#include "stdafx.h"
#include "ExplosionComponent.h"
#include "ExplosionData.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/DeactivateExplosionMessage.h"
#include "PollingStation.h"
#include "ParticleEmitterManager.h"

CExplosionComponent::CExplosionComponent()
{
	myDuration = 1.0f;
	myCollidedWithGameObjects.Init(100);
	myIsActive = false;
}


CExplosionComponent::~CExplosionComponent()
{
}

void CExplosionComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eOnTriggerEnter:
	{
		if(myCollidedWithGameObjects.Find(aMessageData.myComponent->GetParent()) == myCollidedWithGameObjects.FoundNone)
		{
			if(myData->isPlayerFriendly == true && CPollingStation::GetInstance()->CheckIfIsPlayerObject(aMessageData.myComponent->GetParent()) == true)
			{
				break;
			}
			SComponentMessageData damageData;
			damageData.myInt = myData->damage;
			aMessageData.myComponent->GetParent()->NotifyComponents(eComponentMessageType::eTakeDamage, damageData);
			myCollidedWithGameObjects.Add(aMessageData.myComponent->GetParent());
		}
		break;	}
	case eComponentMessageType::eActivateExplosion:
	{

		myData = aMessageData.myExplosionData;
		DoParticles();
		myIsActive = true;
		myElapsedLivingTime = 0.0f;
		myCollidedWithGameObjects.RemoveAll();
		
		break;
	}
	default:
		break;
	}
}
void CExplosionComponent::Destroy()
{

}

void CExplosionComponent::DoParticles()
{

		const CU::Matrix44f transform = GetParent()->GetToWorldTransform();
		

		for (int i = 0; i < myData->explosionParticles.Size(); ++i)
		{
			const Particles::ParticleEmitterID id = CParticleEmitterManager::GetInstance().GetEmitterInstance(myData->explosionParticles[i]);
			CParticleEmitterManager::GetInstance().SetTransformation(id, transform);
			CParticleEmitterManager::GetInstance().Activate(id);

			CParticleEmitterManager::GetInstance().Release(id);
		}
	
}

void CExplosionComponent::Update(float aDeltaTime)
{
	if(myIsActive == true)
	{
		myElapsedLivingTime += aDeltaTime;
		if (myElapsedLivingTime > myDuration)
		{
			GetParent()->NotifyComponents(eComponentMessageType::eDied, SComponentMessageData());
			SComponentMessageData visibilityData;
			visibilityData.myBool = false;
			GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, visibilityData);
			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CDeactivateExplosionMessage(GetParent()));
			myCollidedWithGameObjects.RemoveAll();
			myElapsedLivingTime = 0.0f;
			myIsActive = false;
			GetParent()->SetWorldPosition(CU::Vector3f(0.0f, -10000, 0.0f));
			GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
		}
	}
}

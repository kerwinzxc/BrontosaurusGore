#include "stdafx.h"
#include "DamageOnCollisionComponent.h"
#include "DamageOnCollisionCollidedObjectData.h"
#include "PollingStation.h"

CDamageOnCollisionComponent::CDamageOnCollisionComponent()
{
	myDamage = 0;
	myDamageCooldown = 0.0f;
	myActiveCollidedWithObjectsDataList.Init(10);
	myPassiveCollidedWithObjectsDataList.Init(10);

	for(unsigned char i = 0; i < 10; i++)
	{
		CreateEmptySlotForPassiveBuffer();
	}
}


CDamageOnCollisionComponent::~CDamageOnCollisionComponent()
{
}

void CDamageOnCollisionComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eOnCollisionEnter:
	case eComponentMessageType::eOnTriggerEnter:
		if(CPollingStation::GetInstance()->CheckIfIsPlayerObject(aMessageData.myComponent->GetParent()) == true)
		{
			if (myPassiveCollidedWithObjectsDataList.Size() > 0)
			{
				AddToActiveList(aMessageData.myComponent->GetParent());
				DealDamage(myActiveCollidedWithObjectsDataList.Size() - 1);
			}
			else
			{
				CreateEmptySlotForPassiveBuffer();
				Receive(aMessageType, aMessageData);
			}
		}
		break;
	case eComponentMessageType::eOnCollisionExit:
	case eComponentMessageType::eOnTriggerExit:
	{
		if (CPollingStation::GetInstance()->CheckIfIsPlayerObject(aMessageData.myComponent->GetParent()) == true)
		{
			RemoveFromActiveList(aMessageData.myComponent->GetParent());
		}
		break;
	}
	default:
		break;
	}
}

void CDamageOnCollisionComponent::Update(float aDeltaTime)
{
	for(unsigned short i = 0; i < myActiveCollidedWithObjectsDataList.Size(); i++)
	{
		myActiveCollidedWithObjectsDataList[i]->elapsedDamageCooldownTimer += aDeltaTime;
		if(myActiveCollidedWithObjectsDataList[i]->elapsedDamageCooldownTimer > myDamageCooldown)
		{
			if(myActiveCollidedWithObjectsDataList[i]->collidedObject != nullptr)
			{
				DealDamage(i);
			}
		}
	}
}

void CDamageOnCollisionComponent::CreateEmptySlotForPassiveBuffer()
{
	SDamageOnCollisonCollidedObjectData* newDamageOnCollisonCollidedObjectData = new SDamageOnCollisonCollidedObjectData;
	newDamageOnCollisonCollidedObjectData->collidedObject = nullptr;
	newDamageOnCollisonCollidedObjectData->elapsedDamageCooldownTimer = 0.0f;
	myPassiveCollidedWithObjectsDataList.Add(newDamageOnCollisonCollidedObjectData);
}
void CDamageOnCollisionComponent::DealDamage(unsigned short aIndex)
{
	SComponentMessageData damageData;
	damageData.myInt = myDamage;
	myActiveCollidedWithObjectsDataList[aIndex]->collidedObject->NotifyComponents(eComponentMessageType::eTakeDamage, damageData);
	myActiveCollidedWithObjectsDataList[aIndex]->elapsedDamageCooldownTimer = 0.0f;
}
void CDamageOnCollisionComponent::AddToActiveList(CGameObject* aCollidedObject)
{
	myActiveCollidedWithObjectsDataList.Add(myPassiveCollidedWithObjectsDataList[0]);
	myPassiveCollidedWithObjectsDataList.RemoveCyclicAtIndex(0);
	myActiveCollidedWithObjectsDataList.GetLast()->collidedObject = aCollidedObject;
	myActiveCollidedWithObjectsDataList.GetLast()->elapsedDamageCooldownTimer = 0.0f;
}
void CDamageOnCollisionComponent::RemoveFromActiveList(CGameObject* aNonCollidingObject)
{
		unsigned short index = FindCollidedObjectData(aNonCollidingObject);
		if(index >= 0)
		{
			myPassiveCollidedWithObjectsDataList.Add(myActiveCollidedWithObjectsDataList[index]);
			myActiveCollidedWithObjectsDataList.RemoveCyclicAtIndex(index);
			myPassiveCollidedWithObjectsDataList.GetLast()->collidedObject = nullptr;
			myPassiveCollidedWithObjectsDataList.GetLast()->elapsedDamageCooldownTimer = 0.0f;
		}
			
}

unsigned short CDamageOnCollisionComponent::FindCollidedObjectData(CGameObject* aGameObject)
{
	for (unsigned short i = 0; i < myActiveCollidedWithObjectsDataList.Size(); i++)
	{
		if (myActiveCollidedWithObjectsDataList[i]->collidedObject == aGameObject)
		{
			return i;
		}
	}
	DL_PRINT("No DamageOnCollisonCollidedObject could be found");
	return -1;
}
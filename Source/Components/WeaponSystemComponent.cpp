#include "stdafx.h"
#include "WeaponSystemComponent.h"
#include "Weapon.h"
#include "WeaponFactory.h"
#include "PlayerControls.h"

CWeaponSystemComponent::CWeaponSystemComponent(CWeaponFactory& aWeaponFactoryThatIsGoingToBEHardToObtain)
	:WeaponFactoryPointer(&aWeaponFactoryThatIsGoingToBEHardToObtain)
{

	myActiveWeaponIndex = 0;
	myWeapons.Init(5);
	myIsShooting = false;
	myTemporaryAmmoDataList.Init(5);
}


CWeaponSystemComponent::~CWeaponSystemComponent()
{
}

void CWeaponSystemComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eKeyPressed:
		HandleKeyPressed(aMessageData);
		break;
	case eComponentMessageType::eKeyReleased:
		HandleKeyReleased(aMessageData);
		break;
	//case eComponentMessageType::eTryToShoot:
	//{
	//	myWeapons[myActiveWeaponIndex]->TryToShoot(aMessageData.myVector3f);
	//	break;
	//}
	case eComponentMessageType::eShoot:
	{
		myWeapons[myActiveWeaponIndex]->Shoot(aMessageData.myVector3f);
		break;
	}
	case eComponentMessageType::eAddWeapon:
	{
		WeaponFactoryPointer->CreateWeapon(aMessageData.myString, GetParent());
		break;
	}
	case eComponentMessageType::eWeaponFactoryGiveWeaponToWeaponSystem:
	{
		myWeapons.Add(aMessageData.myWeapon);
		myWeapons.GetLast()->SetUser(GetParent());
		break;
	}
	case eComponentMessageType::eChangeWeapon:
	{
		short index = myActiveWeaponIndex + aMessageData.myInt;
		if (index < 0)
		{
			index = myWeapons.Size() - 1;
		}
		if (index >= myWeapons.Size())
		{
			index = 0;
		}
		myActiveWeaponIndex = static_cast<unsigned int>(index);
		break;
	}
	case eComponentMessageType::eObjectDone:
	{
		for(unsigned int i = 0 ; i < myTemporaryAmmoDataList.Size(); i++)
		{
			SComponentMessageData newAmmoTypeMessage;
			newAmmoTypeMessage.myAmmoData = myTemporaryAmmoDataList[i];
			GetParent()->NotifyOnlyComponents(eComponentMessageType::eAddNewAmmoType, newAmmoTypeMessage);
		}
		myTemporaryAmmoDataList.RemoveAll();
		myTemporaryAmmoDataList.Destroy();
		break;
	}
	default:
		break;
	}
}

void CWeaponSystemComponent::Update(float aDelta)
{
	if (myIsShooting == true)
	{
		SComponentQuestionData lookatData;
		if (GetParent()->AskComponents(eComponentQuestionType::eGetCameraLookat, lookatData) == true)
		{
			myWeapons[myActiveWeaponIndex]->TryToShoot(lookatData.myVector3f);
		}
	}

	for(unsigned short i = 0; i < myWeapons.Size(); i++)
	{
		myWeapons[i]->Update(aDelta);
	}
}

void CWeaponSystemComponent::HandleKeyPressed(const SComponentMessageData& aMessageData)
{
	if (aMessageData.myPlayerControl == ePlayerControls::ePrimaryWeapon)
	{
		myIsShooting = true;
	}
}

void CWeaponSystemComponent::HandleKeyReleased(const SComponentMessageData& aMessageData)
{
	if (aMessageData.myPlayerControl == ePlayerControls::ePrimaryWeapon)
	{
		myIsShooting = false;
	}
}

void CWeaponSystemComponent::GiveWepon(const char* aWeaponName)
{
	WeaponFactoryPointer->CreateWeapon(aWeaponName, GetParent());
}

void CWeaponSystemComponent::AddWeapon(CWeapon* aWeapon, SAmmoData* aTemporaryAmmoData)
{
	myWeapons.Add(aWeapon);
}
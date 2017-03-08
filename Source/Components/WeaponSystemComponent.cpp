#include "stdafx.h"
#include "WeaponSystemComponent.h"
#include "Weapon.h"
#include "WeaponFactory.h"
#include "PlayerControls.h"
#include "AmmoReplenishData.h"
#include "AmmoData.h"
#include "TextInstance.h"
#include "WeaponData.h"

CWeaponSystemComponent::CWeaponSystemComponent(CWeaponFactory& aWeaponFactoryThatIsGoingToBEHardToObtain)
	:WeaponFactoryPointer(&aWeaponFactoryThatIsGoingToBEHardToObtain)
{

	myActiveWeaponIndex = 0;
	myWeapons.Init(5);
	myIsShooting = false;
	myTemporaryAmmoDataList.Init(5);
	myActiveWeaponAmmoLeftText = new CTextInstance();
	myActiveWeaponAmmoLeftText->SetColor(CTextInstance::Red);
	myActiveWeaponAmmoLeftText->SetPosition(CU::Vector2f(0.2f, 0.3f));
	myActiveWeaponAmmoLeftText->SetText("");
	myActiveWeaponAmmoLeftText->Init();
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
		WeaponFactoryPointer->MakeWeaponModel(GetParent(), myWeapons.GetLast());

		break;
	}
	case eComponentMessageType::eChangeWeapon:
	{
		short index = myActiveWeaponIndex + aMessageData.myInt;
		if (index < 0)
		{
			index = myWeapons.Size() - 1;
		}
		else if (static_cast<unsigned int>(index) >= myWeapons.Size())
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
			SAmmoReplenishData ammoReplenishData;
			ammoReplenishData.ammoType = myTemporaryAmmoDataList[i]->ammoForWeaponName.c_str();
			ammoReplenishData.replenishAmount = myTemporaryAmmoDataList[i]->maxAmmo;
			SComponentMessageData giveAmmoData;
			giveAmmoData.myAmmoReplenishData = &ammoReplenishData;
			GetParent()->NotifyComponents(eComponentMessageType::eGiveAmmo, giveAmmoData);
		}
		myTemporaryAmmoDataList.RemoveAll();
		myTemporaryAmmoDataList.Destroy();
		break;
	}
	case eComponentMessageType::eRotateWeaponX:
	{
		for(unsigned int i = 0; i < myWeapons.Size(); i++)
		{
			myWeapons[i]->RotateXAxees(aMessageData.myFloat);
		}
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
	SComponentQuestionData ammoLeftQuestionData;
	ammoLeftQuestionData.myString = myWeapons[myActiveWeaponIndex]->GetData()->name.c_str();
	if(GetParent()->AskComponents(eComponentQuestionType::eGetAmmoLeftString , ammoLeftQuestionData) == true)
	{
		myActiveWeaponAmmoLeftText->SetText(ammoLeftQuestionData.myString);
	}
	myActiveWeaponAmmoLeftText->Render();
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

void CWeaponSystemComponent::GiveWeapon(const char* aWeaponName)
{
	WeaponFactoryPointer->CreateWeapon(aWeaponName, this);
}

void CWeaponSystemComponent::AddWeapon(CWeapon* aWeapon, SAmmoData* aTemporaryAmmoData)
{
	myWeapons.Add(aWeapon);
}
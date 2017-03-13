#include "stdafx.h"
#include "WeaponSystemComponent.h"
#include "Weapon.h"
#include "WeaponFactory.h"
#include "PlayerControls.h"
#include "AmmoReplenishData.h"
#include "AmmoData.h"
#include "TextInstance.h"
#include "WeaponData.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SendNetowrkMessageMessage.h"
#include "../TShared/NetworkMessage_WeaponShoot.h"
#include "../TClient/ClientMessageManager.h"

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
	myActiveWeaponAmmoLeftText->SetText(L"");
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

		CNetworkMessage_WeaponShoot* shootMessage = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_WeaponShoot>("__All");
		
		shootMessage->SetDirection(aMessageData.myVector3f);
		shootMessage->SetWeaponIndex(myActiveWeaponIndex);

		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetowrkMessageMessage(shootMessage));
		break;
	}
	case eComponentMessageType::eShootWithNetworking:
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
		myWeapons[myActiveWeaponIndex]->SetModelVisibility(false);
		if (index < 0)
		{
			index = myWeapons.Size() - 1;
		}
		else if (static_cast<unsigned int>(index) >= myWeapons.Size())
		{
			index = 0;
		}
		myActiveWeaponIndex = static_cast<unsigned int>(index);
		myWeapons[myActiveWeaponIndex]->SetModelVisibility(true);
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
			myWeapons[i]->SetUser(GetParent());
			WeaponFactoryPointer->MakeWeaponModel(GetParent(), myWeapons[i]);
		}
		myTemporaryAmmoDataList.RemoveAll();
		myTemporaryAmmoDataList.Destroy();


		if(myActiveWeaponIndex >= 0 && myActiveWeaponIndex < myWeapons.Size())
		{
			myWeapons[myActiveWeaponIndex]->SetModelVisibility(true);
		}
		break;
	}
	case eComponentMessageType::eSelectWeapon:
	{
		myActiveWeaponIndex = aMessageData.myInt;
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
	SAmmoLeftData ammoLeftData;
	ammoLeftData.weaponName = myWeapons[myActiveWeaponIndex]->GetData()->name.c_str();
	ammoLeftQuestionData.myAmmoLeftData = &ammoLeftData;
	if(GetParent()->AskComponents(eComponentQuestionType::eGetAmmoLeftString , ammoLeftQuestionData) == true)
	{
		std::string ammoLeftText = ammoLeftQuestionData.myAmmoLeftData->weaponName;
		ammoLeftText += ": ";
		ammoLeftText += std::to_string(ammoLeftQuestionData.myAmmoLeftData->ammoLeft);
		ammoLeftText += "/";
		ammoLeftText += std::to_string(ammoLeftQuestionData.myAmmoLeftData->maxAmmo);

		myActiveWeaponAmmoLeftText->SetText(CU::StringToWString(ammoLeftText));
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
	myTemporaryAmmoDataList.Add(aTemporaryAmmoData);
}
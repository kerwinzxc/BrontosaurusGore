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
#include "../FontEngine/FontEngineFacade.h"
#include "../TServer/ServerMessageManager.h"
#include "../TShared/NetworkMessage_WeaponChange.h"

CWeaponSystemComponent::CWeaponSystemComponent(CWeaponFactory& aWeaponFactoryThatIsGoingToBEHardToObtain)
	:WeaponFactoryPointer(&aWeaponFactoryThatIsGoingToBEHardToObtain)
{

	myActiveWeaponIndex = 0;
	myWeapons.Init(5);
	myIsShooting = false;
	myTemporaryAmmoDataList.Init(5);

	if(CFontEngineFacade::GetIsCreated() == true)
	{
		myActiveWeaponAmmoLeftText = new CTextInstance();
		myActiveWeaponAmmoLeftText->SetColor(CTextInstance::Red);
		myActiveWeaponAmmoLeftText->SetPosition(CU::Vector2f(0.2f, 0.3f));
		myActiveWeaponAmmoLeftText->SetText(L"");
		myActiveWeaponAmmoLeftText->Init();
	}
	myIsActive = true;
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
		if (myIsActive == true)
		{
			myWeapons[myActiveWeaponIndex]->Shoot(aMessageData.myVector3f);

			CNetworkMessage_WeaponShoot* shootMessage = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_WeaponShoot>("__All");

			shootMessage->SetDirection(aMessageData.myVector3f);
			shootMessage->SetWeaponIndex(myActiveWeaponIndex);
			shootMessage->SetShooter(CNetworkMessage_WeaponShoot::Shooter::Player);

			Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(shootMessage));
		}
		break;
	}
	case eComponentMessageType::eServerShoot:
	{
		if(myIsActive == true)
		{
			if(myWeapons[myActiveWeaponIndex]->CanShoot() == true)
			{
				myWeapons[myActiveWeaponIndex]->Shoot(aMessageData.myVector3f);

				CNetworkMessage_WeaponShoot* shootMessage = CServerMessageManager::GetInstance()->CreateMessage<CNetworkMessage_WeaponShoot>(ID_ALL);

				shootMessage->SetDirection(aMessageData.myVector3f);
				shootMessage->SetShooter(CNetworkMessage_WeaponShoot::Shooter::Enemy);
				shootMessage->SetShooterId(aMessageData.myVector4f.w);
				shootMessage->SetWeaponIndex(myActiveWeaponIndex);

				Postmaster::Threaded::CPostmaster::GetInstance().BroadcastLocal(new CSendNetworkMessageMessage(shootMessage));
			
			}
		
		}
		break;
	}
	case eComponentMessageType::eShootWithNetworking:
	{
		myWeapons[myActiveWeaponIndex]->CosmeticShoot(aMessageData.myVector3f);
		break;
	}
	case eComponentMessageType::eAddWeapon:
	{
		unsigned short theWeaponIndexOfAddedweapon = WeaponFactoryPointer->CreateWeapon(aMessageData.myString, GetParent());
		break;
	}
	case eComponentMessageType::eAddWeaponIndex:
	{
		WeaponFactoryPointer->CreateWeapon(aMessageData.myInt, GetParent());
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
		if(myIsActive == true)
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
			ChangeWeapon(index);
		
		}
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
		ChangeWeapon(aMessageData.myInt);
		break;
	}
	case eComponentMessageType::eServerChangeWeapon:
		ChangeWeaponLocal(aMessageData.myInt);
		break;
	case eComponentMessageType::eDied:
	{
		myIsActive = false;
		break;
	}
	case eComponentMessageType::eCheckPointReset:
	{
		myIsActive = true;
		break;
	}
	default:
		break;
	}
}

bool CWeaponSystemComponent::WeaponIndexValid() const
{
	return myActiveWeaponIndex < myWeapons.Size();
}

void CWeaponSystemComponent::Update(float aDelta)
{
	if (myIsShooting == true)
	{
		if(myIsActive == true)
		{
			SComponentQuestionData lookatData;
			if (GetParent()->AskComponents(eComponentQuestionType::eGetCameraLookat, lookatData) == true)
			{
				myWeapons[myActiveWeaponIndex]->TryToShoot(lookatData.myVector3f);
			}
		
		}
	}

	for(unsigned short i = 0; i < myWeapons.Size(); i++)
	{
		myWeapons[i]->Update(aDelta);
	}
	SComponentQuestionData ammoLeftQuestionData;
	SAmmoLeftData ammoLeftData;

	if (CFontEngineFacade::GetIsCreated() == true)
	{
		if(WeaponIndexValid() == true)
		{
			ammoLeftData.weaponName = myWeapons[myActiveWeaponIndex]->GetData()->name.c_str();
			ammoLeftQuestionData.myAmmoLeftData = &ammoLeftData;
			if (GetParent()->AskComponents(eComponentQuestionType::eGetAmmoLeftString, ammoLeftQuestionData) == true)
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
		
	}
}

void CWeaponSystemComponent::HandleKeyPressed(const SComponentMessageData& aMessageData)
{
	switch (aMessageData.myPlayerControl)
	{
	case ePlayerControls::ePrimaryWeapon :
		{
			myIsShooting = true;
			break;
		}
	case ePlayerControls::eChangeWeapon1:
	{
		ChangeWeapon(0);
		break;
	}
	case ePlayerControls::eChangeWeapon2:
	{
		ChangeWeapon(1);
		break;
	}
	case ePlayerControls::eChangeWeapon3:
	{
		ChangeWeapon(2);
		break;
	}
	case ePlayerControls::eChangeWeapon4:
	{
		ChangeWeapon(3);
		break;
	}
	default:
		break;
	}

}

void CWeaponSystemComponent::HandleKeyReleased(const SComponentMessageData& aMessageData)
{
	if (aMessageData.myPlayerControl == ePlayerControls::ePrimaryWeapon)
	{
		myIsShooting = false;
	}
}

void CWeaponSystemComponent::ChangeWeapon(unsigned aIndex)
{
	ChangeWeaponLocal(aIndex);
	
	CNetworkMessage_WeaponChange* changeMessage = CClientMessageManager::GetInstance()->CreateMessage<CNetworkMessage_WeaponChange>("__All");
	changeMessage->SetWeapon(myActiveWeaponIndex);
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new CSendNetworkMessageMessage(changeMessage));
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

void CWeaponSystemComponent::ChangeWeaponLocal(unsigned int aIndex)
{
	if(aIndex >= 0 && aIndex < myWeapons.Size())
	{
		if (myIsActive == true)
		{
			myWeapons[myActiveWeaponIndex]->SetModelVisibility(false);
			myActiveWeaponIndex = aIndex;
			myWeapons[myActiveWeaponIndex]->SetModelVisibility(true);
		}
	}
}

bool CWeaponSystemComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	switch (aQuestionType)
	{
	case eComponentQuestionType::eCanShoot :
	{
		return myWeapons[myActiveWeaponIndex]->CanShoot();
	}
	case eComponentQuestionType::eGetWeapons:
	{
		aQuestionData.myWeapons = &myWeapons;
		return true;
	}
	default:
		break;
	}
	return false;
}
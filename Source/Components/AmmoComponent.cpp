#include "stdafx.h"
#include "AmmoComponent.h"
#include "AmmoData.h"
#include "GeneralAmmoData.h"
#include "AmmoCheckData.h"
#include "AmmoReplenishData.h"

CAmmoComponent::CAmmoComponent()
{
	mySelectedAmmoType = -1;
	myGeneralAmmoDataList.Init(5);
}


CAmmoComponent::~CAmmoComponent()
{
}

bool CAmmoComponent::AmmoIndexValid() const
{
	return mySelectedAmmoType != -1 && mySelectedAmmoType < myGeneralAmmoDataList.Size();
}

void CAmmoComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eCheckIfHaveAmmoForShooting:
	{
		ChangeSelectedAmmoType(aMessageData.myAmmoCheckData->ammoType);
		if (mySelectedAmmoType >= 0 && mySelectedAmmoType < myGeneralAmmoDataList.Size())
		{
			if (myGeneralAmmoDataList[mySelectedAmmoType]->currentAmmoAmount > 0)
			{
				myGeneralAmmoDataList[mySelectedAmmoType]->currentAmmoAmount--;
				SComponentMessageData shootData;
				shootData.myVector3f = aMessageData.myAmmoCheckData->shootingDirection;
				GetParent()->NotifyComponents(eComponentMessageType::eShoot, shootData);
			}
		}
		else
		{
			DL_PRINT("Something went wrong when trying to shoot. Tell Marcus and ask him to fix it");
		}
		break;
	}
	case eComponentMessageType::eAddNewAmmoType:
	{
		SGeneralAmmoData* newGeneralAmmoData = new SGeneralAmmoData();
		newGeneralAmmoData->currentAmmoAmount = 0;
		newGeneralAmmoData->ammoTypeData = aMessageData.myAmmoData;
		myGeneralAmmoDataList.Add(newGeneralAmmoData);
		mySelectedAmmoType = myGeneralAmmoDataList.Size() - 1;
		break;
	}
	case eComponentMessageType::eGiveAmmo:
	{
		if(AmmoIndexValid() == true)
		{
			ChangeSelectedAmmoType(aMessageData.myAmmoReplenishData->ammoType.c_str());
			if (myGeneralAmmoDataList[mySelectedAmmoType]->currentAmmoAmount + aMessageData.myAmmoReplenishData->replenishAmount > myGeneralAmmoDataList[mySelectedAmmoType]->ammoTypeData->maxAmmo)
			{
				myGeneralAmmoDataList[mySelectedAmmoType]->currentAmmoAmount = myGeneralAmmoDataList[mySelectedAmmoType]->ammoTypeData->maxAmmo;
			}
			else
			{
				myGeneralAmmoDataList[mySelectedAmmoType]->currentAmmoAmount += aMessageData.myAmmoReplenishData->replenishAmount;
			}
		}
		
		break;
	}
	default:
		break;
	}
}

void CAmmoComponent::Destroy()
{
}

void CAmmoComponent::Update(float /*aDeltaTime*/)
{
	//this function doesn't do anthing anymore :/;
}

void CAmmoComponent::ChangeSelectedAmmoType(const char* aAmmoType)
{
	for (unsigned int i = 0; i < myGeneralAmmoDataList.Size(); i++)
	{
		if (myGeneralAmmoDataList[i]->ammoTypeData->ammoForWeaponName == aAmmoType)
		{
			mySelectedAmmoType = i;
			return;
		}
	}
	DL_PRINT("Couldn't find what to select as ammo. This might cause some bugs.");
}

bool CAmmoComponent::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	switch (aQuestionType)
	{
	case eComponentQuestionType::eGetAmmoLeftString:
	{
		ChangeSelectedAmmoType(aQuestionData.myAmmoLeftData->weaponName);
		aQuestionData.myAmmoLeftData->ammoLeft = myGeneralAmmoDataList[mySelectedAmmoType]->currentAmmoAmount;
		aQuestionData.myAmmoLeftData->maxAmmo = myGeneralAmmoDataList[mySelectedAmmoType]->ammoTypeData->maxAmmo;
		return true;
	}
	case eComponentQuestionType::eGetCurrentAmmoData:
	{
		aQuestionData.myAmmoLeftData->weaponName = myGeneralAmmoDataList.At(mySelectedAmmoType)->ammoTypeData->ammoForWeaponName.c_str();
		aQuestionData.myAmmoLeftData->ammoLeft = myGeneralAmmoDataList.At(mySelectedAmmoType)->currentAmmoAmount;
		aQuestionData.myAmmoLeftData->maxAmmo = myGeneralAmmoDataList.At(mySelectedAmmoType)->ammoTypeData->maxAmmo;
	}
	default:
		break;
	}
	return false;
}
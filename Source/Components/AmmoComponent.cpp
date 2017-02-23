#include "stdafx.h"
#include "AmmoComponent.h"
#include "AmmoData.h"
#include "GeneralAmmoData.h"

CAmmoComponent::CAmmoComponent()
{
	mySelectedAmmoType = -1;
	myGeneralAmmoDataList.Init(5);
}


CAmmoComponent::~CAmmoComponent()
{
}

void CAmmoComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eCheckIfHaveAmmoForShooting:
			if(mySelectedAmmoType >= 0 && mySelectedAmmoType < myGeneralAmmoDataList.Size())
			{
				if (myGeneralAmmoDataList[mySelectedAmmoType]->currentAmmoAmount > 0)
				{
					myGeneralAmmoDataList[mySelectedAmmoType]->currentAmmoAmount--;
					GetParent()->NotifyComponents(eComponentMessageType::eShoot, aMessageData);
				}	
			}
			else
			{
				DL_PRINT("Something went wrong when trying to shoot. Tell Marcus and ask him to fix it");
			}
		break;
	case eComponentMessageType::eAddNewAmmoType:
	{
		SGeneralAmmoData* newGeneralAmmoData = new SGeneralAmmoData();
		newGeneralAmmoData->currentAmmoAmount = 0;
		newGeneralAmmoData->ammoTypeData = aMessageData.myAmmoData;
		myGeneralAmmoDataList.Add(newGeneralAmmoData);
		break;
	}
	case eComponentMessageType::eChangeSelectedAmmoType:
	{
		for(unsigned int i = 0; i < myGeneralAmmoDataList.Size(); i++)
		{
			if(myGeneralAmmoDataList[i]->ammoTypeData->ammoForWeaponName == aMessageData.myString)
			{
				mySelectedAmmoType = i;
				break;
			}
		}	
		break;
	}
	case eComponentMessageType::eGiveAmmo:
	{
		if(myGeneralAmmoDataList[mySelectedAmmoType]->currentAmmoAmount += aMessageData.myInt > myGeneralAmmoDataList[mySelectedAmmoType]->ammoTypeData->maxAmmo)
		{
			myGeneralAmmoDataList[mySelectedAmmoType]->currentAmmoAmount = myGeneralAmmoDataList[mySelectedAmmoType]->ammoTypeData->maxAmmo;
		}
		else
		{
			myGeneralAmmoDataList[mySelectedAmmoType]->currentAmmoAmount += aMessageData.myInt;
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

void CAmmoComponent::Update(float aDeltaTime)
{
	aDeltaTime;
	//this function doesn't do anthing anymore :/;
}
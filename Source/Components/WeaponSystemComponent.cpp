#include "stdafx.h"
#include "WeaponSystemComponent.h"
#include "Weapon.h"
#include "WeaponFactory.h"

WeaponSystemComponent::WeaponSystemComponent(WeaponFactory& aWeaponFactoryThatIsGoingToBEHardToObtain)
	:WeaponFactoryPointer(&aWeaponFactoryThatIsGoingToBEHardToObtain)
{

	myActiveWeaponIndex = 0;
	myWeapons.Init(5);
}


WeaponSystemComponent::~WeaponSystemComponent()
{
}

void WeaponSystemComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eTryToShoot:
		myWeapons[myActiveWeaponIndex]->TryToShoot(aMessageData.myVector3f);
		break;
	case eComponentMessageType::eShoot:
		myWeapons[myActiveWeaponIndex]->Shoot(aMessageData.myVector3f);
		break;
	case eComponentMessageType::eAddWeapon:
		WeaponFactoryPointer->CreateWeapon(aMessageData.myString, GetParent());
		break;
	case eComponentMessageType::eWeaponFactoryGiveWeaponToWeaponSystem:
		myWeapons.Add(aMessageData.myWeapon);
		myWeapons.GetLast()->SetUser(GetParent());
		break;
	default:
		break;
	}
}

void WeaponSystemComponent::Destroy()
{
}

void WeaponSystemComponent::Update(float aDelta)
{
	for(unsigned short i = 0; i < myWeapons.Size(); i++)
	{
		myWeapons[i]->Update(aDelta);
	}
}
#include "stdafx.h"
#include "Weapon.h"
#include "ProjectileFactory.h"
#include "WeaponData.h"

Weapon::Weapon(WeaponData* aWeaponData)
{
	myElapsedFireTimer = 0.0f;
}


Weapon::~Weapon()
{
}

void Weapon::TryToShoot(const CU::Vector3f& aDirection)
{
	if (myElapsedFireTimer >= myWeaponData->fireRate)
	{
		SComponentMessageData selectAmmoTypeData;
		selectAmmoTypeData.myString = myWeaponData->name.c_str();
		myUser->NotifyOnlyComponents(eComponentMessageType::eChangeSelectedAmmoType, selectAmmoTypeData);
		SComponentMessageData directionData;
		directionData.myVector3f = aDirection;
		myUser->NotifyComponents(eComponentMessageType::eCheckIfHaveAmmoForShooting, directionData);
	
	}
}
void Weapon::Shoot(const CU::Vector3f& aDirection)
{
	if (myElapsedFireTimer >= myWeaponData->fireRate)
	{
		ProjectileFactory::GetInstance().ShootProjectile(myWeaponData->projectileData, aDirection, myUser->GetWorldPosition());
		myElapsedFireTimer = 0.0f;
	}
}
void Weapon::Update(float aDeltaTime)
{
	if(myElapsedFireTimer < myWeaponData->fireRate)
	{
		myElapsedFireTimer += aDeltaTime;
	}
}
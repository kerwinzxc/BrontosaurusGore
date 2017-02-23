#include "stdafx.h"
#include "Weapon.h"
#include "ProjectileFactory.h"
#include "WeaponData.h"

CWeapon::CWeapon(SWeaponData* aWeaponData)
{
	myElapsedFireTimer = 0.0f;
	myWeaponData = aWeaponData;
}


CWeapon::~CWeapon()
{
}

void CWeapon::TryToShoot(const CU::Vector3f& aDirection)
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
void CWeapon::Shoot(const CU::Vector3f& aDirection)
{
	if (myElapsedFireTimer >= myWeaponData->fireRate)
	{
		CProjectileFactory::GetInstance().ShootProjectile(myWeaponData->projectileData, aDirection, myUser->GetWorldPosition());
		myElapsedFireTimer = 0.0f;
	}
}
void CWeapon::Update(float aDeltaTime)
{
	if(myElapsedFireTimer < myWeaponData->fireRate)
	{
		myElapsedFireTimer += aDeltaTime;
	}
}
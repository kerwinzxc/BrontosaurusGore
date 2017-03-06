#include "stdafx.h"
#include "Weapon.h"
#include "ProjectileFactory.h"
#include "WeaponData.h"

CWeapon::CWeapon(SWeaponData* aWeaponData)
{
	myElapsedFireTimer = 0.0f;
	myWeaponData = aWeaponData;
	myUser = nullptr;
	myWeaponObject = nullptr;
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
		for (unsigned short i = 0; i < myWeaponData->projectilesFiredPerShot; i++)
		{
			CU::Vector3f rotatedDirection = aDirection;
			rotatedDirection = aDirection;
			if(myWeaponData->randomSpreadAngleX > 0)
			{
				float randomNumber = static_cast<float>(rand() % myWeaponData->randomSpreadAngleX);
				float randomXAngle = randomNumber - static_cast<float>(myWeaponData->randomSpreadAngleX) / 2.0f;
				randomNumber = static_cast<float>(rand() % myWeaponData->randomSpreadAngleY);
				float randomXRadian = randomXAngle * PI / 180.0f;
				rotatedDirection = rotatedDirection * CU::Matrix33f::CreateRotateAroundY(randomXRadian);
			}
			if (myWeaponData->randomSpreadAngleY > 0)
			{
				float randomNumber = static_cast<float>(rand() % myWeaponData->randomSpreadAngleY);
				float randomYAngle = randomNumber - static_cast<float>(myWeaponData->randomSpreadAngleY) / 2.0f;
				float randomYRadian = randomYAngle * PI / 180.0f;
				rotatedDirection = rotatedDirection * CU::Matrix33f::CreateRotateAroundX(randomYRadian);
			}
			if(CProjectileFactory::GetInstance() != nullptr)
			{
				CProjectileFactory::GetInstance()->ShootProjectile(myWeaponData->projectileData, rotatedDirection, myUser->GetWorldPosition());
				myElapsedFireTimer = 0.0f;
			
			}
		}
	}
}
void CWeapon::Update(float aDeltaTime)
{
	if(myElapsedFireTimer < myWeaponData->fireRate)
	{
		myElapsedFireTimer += aDeltaTime;
	}
}

void CWeapon::RotateXAxees(const float aRotationAmount)
{
	if(myWeaponObject != nullptr)
	{
		myWeaponObject->GetLocalTransform().Rotate(aRotationAmount, CU::Axees::X);	
	}
}
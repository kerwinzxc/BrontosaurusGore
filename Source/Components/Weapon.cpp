#include "stdafx.h"
#include "Weapon.h"
#include "ProjectileFactory.h"
#include "WeaponData.h"
#include "AmmoCheckData.h"

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

		SComponentMessageData directionData;
		SAmmoCheckData tempAmmoCheckData;
		tempAmmoCheckData.ammoType = myWeaponData->name.c_str();
		tempAmmoCheckData.shootingDirection = aDirection;
		directionData.myAmmoCheckData = &tempAmmoCheckData;
		myUser->NotifyComponents(eComponentMessageType::eCheckIfHaveAmmoForShooting, directionData);
	
	}
}
void CWeapon::Shoot(const CU::Vector3f& aDirection)
{
	if (myElapsedFireTimer >= myWeaponData->fireRate)
	{
		CU::Vector3f shootPosition = myUser->GetWorldPosition();
		SComponentQuestionData cameraPositionData;
		if (myUser->AskComponents(eComponentQuestionType::eGetCameraPosition, cameraPositionData))
		{
			shootPosition = cameraPositionData.myVector3f;
			shootPosition += CU::Vector3f(0.f, 0.f, 5.f) * myUser->GetToWorldTransform().GetRotation();
		}

		for (unsigned short i = 0; i < myWeaponData->projectilesFiredPerShot; i++)
		{
			CU::Vector3f rotatedDirection = aDirection.GetNormalized();
			rotatedDirection.Normalize();
			rotatedDirection.Print();
			CU::Vector2f rotatedRadians;
			if(myWeaponData->randomSpreadAngleX > 0)
			{
				float randomNumber = static_cast<float>(rand() % myWeaponData->randomSpreadAngleX);
				float randomReducer = (rand() % 100 + 1) / 100.0f;
				randomNumber *= randomReducer;
				float randomXAngle = randomNumber - static_cast<float>(myWeaponData->randomSpreadAngleX) / 2.0f * randomReducer;
				randomNumber = static_cast<float>(rand() % myWeaponData->randomSpreadAngleY);
				float randomXRadian = randomXAngle * PI / 180.0f;
				rotatedRadians.x = randomXRadian;
				
				//rotatedDirection = rotatedDirection * CU::Matrix33f::CreateRotateAroundY(randomXRadian);
			}
			if (myWeaponData->randomSpreadAngleY > 0)
			{
				float randomNumber = static_cast<float>(rand() % myWeaponData->randomSpreadAngleY);
				float randomReducer = (rand() % 100 + 1) / 100.0f;
				randomNumber *= randomReducer;
				float randomYAngle = randomNumber - static_cast<float>(myWeaponData->randomSpreadAngleY) / 2.0f * randomReducer;
				float randomYRadian = randomYAngle * PI / 180.0f;
				rotatedRadians.y = randomYRadian;
				
				//rotatedDirection = rotatedDirection * CU::Matrix33f::CreateRotateAroundX(randomYRadian);
			}

			float rotatedAngleLEnght =  rotatedRadians.Length2() * 180.0f / PI;
			if(static_cast<float>(myWeaponData->randomSpreadAngleY) / 2.0f *static_cast<float>(myWeaponData->randomSpreadAngleY) / 2.0f< rotatedRadians.Length2() * 180.0f / PI * 180.0f / PI)
			{
				rotatedRadians.Normalize();
				rotatedRadians *= static_cast<float>(myWeaponData->randomSpreadAngleY) / 2.0f * PI / 180.0f;
			}
			//CU::Matrix44f
			
			rotatedDirection = rotatedDirection * CU::Matrix33f::CreateRotateAroundY(rotatedRadians.x);
			rotatedDirection = rotatedDirection * CU::Matrix33f::CreateRotateAroundX(rotatedRadians.y);
			rotatedDirection.Normalize();
			if(CProjectileFactory::GetInstance() != nullptr)
			{
				CProjectileFactory::GetInstance()->ShootProjectile(myWeaponData->projectileData, rotatedDirection, /*myUser->GetWorldPosition()*/shootPosition);
				myElapsedFireTimer = 0.0f;
			
			}
		}

		if(myWeaponData->shouldRayCast == true)
		{
			//DO RayCastStuff plz
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
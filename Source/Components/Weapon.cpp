#include "stdafx.h"
#include "Weapon.h"
#include "ProjectileFactory.h"
#include "WeaponData.h"
#include "AmmoCheckData.h"
#include "ProjectileData.h"

#include "../Physics/PhysicsScene.h"
#include "../Physics/PhysicsActor.h"
#include "../Physics/PhysicsActorDynamic.h"
#include "../Audio/AudioInterface.h"
#include "ParticleEmitterManager.h"
#include "ParticleEmitterComponent.h"
#include "../TClient/ClientMessageManager.h"
#include "..\CommonUtilities\DL_Debug.h"

CWeapon::CWeapon(SWeaponData* aWeaponData, Physics::CPhysicsScene* aPhysicsScene) : myAudioId(0)
{
	myElapsedFireTimer = 0.0f;
	myWeaponData = aWeaponData;
	myUser = nullptr;
	myWeaponObject = nullptr;
	myPhysicsScene = aPhysicsScene;
	mySoundDirection = { 0.0f, 0.0f, 1.0f };
	myDeltaTime = 0.0f;
	myClickSoundCoolDown = 0.0f;
	if (Audio::CAudioInterface::GetInstance() != nullptr)
	{
		myAudioId = Audio::CAudioInterface::GetInstance()->RegisterGameObject();
	}
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

void CWeapon::Shoot(const CU::Vector3f& aDirection, const bool aHaveAmmo)
{
	if (myElapsedFireTimer >= myWeaponData->fireRate)
	{
		if (aHaveAmmo == true)
		{
			CU::Matrix44f transform = myUser->GetToWorldTransform();
			//CU::Vector3f shootPosition = myUser->GetWorldPosition();
			CU::Vector3f cameraPosition = transform.GetPosition();
			SComponentQuestionData cameraPositionData;
			if (myUser->AskComponents(eComponentQuestionType::eGetCameraPosition, cameraPositionData))
			{
				CU::Vector3f position = cameraPositionData.myVector3f;
				cameraPosition = position;
				position += CU::Vector3f(0.f, 0.f, 0.55f) * myUser->GetToWorldTransform().GetRotation();
				transform.SetPosition(position);
			}

			for (unsigned short i = 0; i < myWeaponData->projectilesFiredPerShot; i++)
			{
				CU::Vector3f direction = RandomizedDirection(aDirection);
				direction.Normalize();

				if (myWeaponData->projectileData->shouldRayCast == true)
				{
					Physics::SRaycastHitData hitData;
					if (myWeaponObject != nullptr)
					{
						hitData = myPhysicsScene->Raycast(transform.GetPosition(), direction, myWeaponData->projectileData->maximumTravelRange);
					}
					else
					{
						hitData = myPhysicsScene->Raycast(transform.GetPosition(), direction, myWeaponData->projectileData->maximumTravelRange);
					}
					if (hitData.hit == true)
					{
						const Physics::EActorType actorType = hitData.actor->GetType();
						myLastHitNormal = hitData.normal;
						myLastHitPosition = hitData.position;
						/*if (actorType == Physics::EActorType::eDynamic)
						{
							static_cast<Physics::CPhysicsActorDynamic*>(hitData.actor)->AddForce(aDirection * 1000);
							static_cast<Physics::CPhysicsActorDynamic*>(hitData.actor)->AddTorque(aDirection * 1000);
						};*/

						CGameObject* gameObject = static_cast<CComponent*>(hitData.actor->GetCallbackData()->GetUserData())->GetParent();
						if (gameObject != myUser)
						{
							//SComponentQuestionData healthLeftData;
							//if (gameObject->AskComponents(eComponentQuestionType::eGetHealth, healthLeftData) == true)
							//{
							//	if (healthLeftData.myInt <= 0)
							//	{
							//		DL_PRINT("Removing dead shit");
							//		CU::Vector3f hellPosition(0.0f, 0.0f, 0.0f);
							//		gameObject->GetLocalTransform().SetPosition(hellPosition);
							//		SComponentMessageData positioonData;
							//		positioonData.myVector3f = hellPosition;
							//		gameObject->NotifyComponents(eComponentMessageType::eSetControllerPosition, positioonData);
							//		gameObject->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
							//		Shoot(aDirection);
							//		return;
							//	}
							//}
							SComponentMessageData damageData;

							damageData.myVector3f = myLastHitNormal;
							gameObject->NotifyComponents(eComponentMessageType::eSetLastHitNormal, damageData);
							damageData.myVector3f = myLastHitPosition;
							gameObject->NotifyComponents(eComponentMessageType::eSetLastHitPosition, damageData);

							damageData.myVector4f.y = direction.x;
							damageData.myVector4f.z = direction.y;
							damageData.myVector4f.w = direction.z;
							damageData.myInt = myWeaponData->projectileData->damage;
							gameObject->NotifyComponents(eComponentMessageType::eTakeDamage, damageData);

							Audio::CAudioInterface* audio = Audio::CAudioInterface::GetInstance();
							audio->PostEvent("Impact_Tick");

						}
					}
				}

				myElapsedFireTimer = 0.0f;
				/*rotatedDirection = rotatedDirection * CU::Matrix33f::CreateRotateAroundY(rotatedRadians.x);
				rotatedDirection = rotatedDirection * CU::Matrix33f::CreateRotateAroundX(rotatedRadians.y);
				rotatedDirection.Normalize();*/
				if (CProjectileFactory::GetInstance() != nullptr)
				{
					CU::Vector3f shootDisplacment(myWeaponData->shootPositionX, myWeaponData->shootPositionY, myWeaponData->shootPositionZ);
					if (myWeaponObject != nullptr)
					{
						CU::Vector3f position = myWeaponObject->GetWorldPosition();
						CU::Matrix44f localWeaponMatrix = myWeaponObject->GetToWorldTransform();
						localWeaponMatrix.Move(shootDisplacment);
						position = localWeaponMatrix.GetPosition();
						transform.SetPosition(position);
					}

					PlaySound(SoundEvent::Fire, direction);
					CProjectileFactory::GetInstance()->ShootProjectile(myWeaponData->projectileData, direction, /*myUser->GetWorldPosition()*/transform.GetPosition());



				}
			}
			if (CClientMessageManager::GetInstance() != nullptr)
			{
				transform.LookAt(transform.GetPosition() + aDirection);
				EmitParticles(transform);
			}
		}
		else
		{
			if (myClickSoundCoolDown <= 0)
			{
				if (myWeaponData->name == "Shotgun")
				{
					myClickSoundCoolDown = myWeaponData->fireRate - 0.3f;
				}
				else
				{
					myClickSoundCoolDown = myWeaponData->fireRate;
				}
				Audio::CAudioInterface::GetInstance()->PostEvent("Player_OutOfAmmo");
			}
		}
	}

}
void CWeapon::Update(float aDeltaTime)
{
	myDeltaTime = aDeltaTime;
	if (myClickSoundCoolDown > 0)
	{
		myClickSoundCoolDown -= aDeltaTime;
	}
	if (myAudioId != 0)
	{
		const CU::Matrix44f transform = myUser->GetToWorldTransform();
		Audio::CAudioInterface::GetInstance()->SetGameObjectPosition(myAudioId, transform.GetPosition() + mySoundDirection, mySoundDirection.GetNormalized());
	}
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

void CWeapon::SetModelVisibility(bool aVisibility)
{
	if (myWeaponObject != nullptr)
	{
		SComponentMessageData visibilityMessage;
		visibilityMessage.myBool = aVisibility;
		myWeaponObject->NotifyOnlyComponents(eComponentMessageType::eSetVisibility, visibilityMessage);
	}
}

void CWeapon::CosmeticShoot(const CU::Vector3f & aDirection)
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
		CU::Matrix44f localWeaponMatrix;
		for (int i = 0; i < myWeaponData->projectilesFiredPerShot; ++i)
		{
			CU::Vector3f direction = RandomizedDirection(aDirection); // might wanna change this later to some raycasting stuff
			direction.Normalize();

			/*rotatedDirection = rotatedDirection * CU::Matrix33f::CreateRotateAroundY(rotatedRadians.x);
		rotatedDirection = rotatedDirection * CU::Matrix33f::CreateRotateAroundX(rotatedRadians.y);
		rotatedDirection.Normalize();*/
			if (CProjectileFactory::GetInstance() != nullptr)
			{
				CU::Vector3f shootDisplacment(myWeaponData->shootPositionX, myWeaponData->shootPositionY, myWeaponData->shootPositionZ);
			
				shootPosition = myUser->GetWorldPosition();
				localWeaponMatrix = myUser->GetToWorldTransform();
				localWeaponMatrix.Move(shootDisplacment);
				shootPosition = localWeaponMatrix.GetPosition();


				PlaySound(SoundEvent::Fire, aDirection);
				CProjectileFactory::GetInstance()->ShootProjectile(myWeaponData->projectileData, direction, /*myUser->GetWorldPosition()*/shootPosition);
				
				myElapsedFireTimer = 0.0f;
			}
		}
		if(CClientMessageManager::GetInstance() != nullptr)
		{
			localWeaponMatrix.LookAt(localWeaponMatrix.GetPosition() + aDirection);
			EmitParticles(localWeaponMatrix);
		}
		
	}
}

const CU::Vector3f& CWeapon::GetLastHitNormal() const
{
	return myLastHitNormal;
}

const CU::Vector3f& CWeapon::GetLastHitPosition() const
{
	return myLastHitPosition;
}


void CWeapon::EmitParticles(CU::Matrix44f aMatrix44)
{
	
	aMatrix44.SetPosition(aMatrix44.GetPosition() + aMatrix44.myForwardVector * 2.5);
	aMatrix44.myForwardVector *= -1;
	
	for(int i =  0; i < myWeaponData->fireParticles.Size(); ++i)
	{
		Particles::ParticleEmitterID id = CParticleEmitterManager::GetInstance().GetEmitterInstance(myWeaponData->fireParticles[i]);
		CParticleEmitterManager::GetInstance().SetTransformation(id, aMatrix44);
		CParticleEmitterManager::GetInstance().Activate(id);

		CParticleEmitterManager::GetInstance().Release(id);
	}
}


void CWeapon::PlaySound(SoundEvent aSoundEvent, const CU::Vector3f& aDirection)
{
	mySoundDirection = aDirection;
	std::string eventId;

	switch (aSoundEvent)
	{
	case SoundEvent::Fire: 
		eventId = myWeaponData->soundData.fire;
		break;
	case SoundEvent::Reload: 
		eventId = myWeaponData->soundData.reload;
		break;
	default: break;
	}
	if(eventId.empty() == false)
	{
		

		Audio::CAudioInterface::GetInstance()->PostEvent(eventId.c_str(),myAudioId);
	}
}


CU::Vector3f CWeapon::RandomizedDirection(const CU::Vector3f& aDirection)
{
	CU::Vector3f rotatedDirection = aDirection.GetNormalized();
	rotatedDirection.Normalize();
	CU::Vector2f rotatedRadians;
	if (myWeaponData->randomSpreadAngle > 0)
	{
		float randomNumber = static_cast<float>(rand() % myWeaponData->randomSpreadAngle);
		float randomReducer = (rand() % 100 + 1) / 100.0f;
		randomNumber *= randomReducer;
		float randomXAngle = randomNumber - static_cast<float>(myWeaponData->randomSpreadAngle) / 2.0f * randomReducer;
		randomNumber = static_cast<float>(rand() % myWeaponData->randomSpreadAngle);
		float randomXRadian = randomXAngle * PI / 180.0f;
		rotatedRadians.x = randomXRadian;

		//rotatedDirection = rotatedDirection * CU::Matrix33f::CreateRotateAroundY(randomXRadian);
	}
	if (myWeaponData->randomSpreadAngle > 0)
	{
		float randomNumber = static_cast<float>(rand() % myWeaponData->randomSpreadAngle);
		float randomReducer = (rand() % 100 + 1) / 100.0f;
		randomNumber *= randomReducer;
		float randomYAngle = randomNumber - static_cast<float>(myWeaponData->randomSpreadAngle) / 2.0f * randomReducer;
		float randomYRadian = randomYAngle * PI / 180.0f;
		rotatedRadians.y = randomYRadian;

		//rotatedDirection = rotatedDirection * CU::Matrix33f::CreateRotateAroundX(randomYRadian);
	}

	float rotatedAngleLEnght = rotatedRadians.Length2() * 180.0f / PI;
	if (static_cast<float>(myWeaponData->randomSpreadAngle) / 2.0f *static_cast<float>(myWeaponData->randomSpreadAngle) / 2.0f< rotatedRadians.Length2() * 180.0f / PI * 180.0f / PI)
	{
		rotatedRadians.Normalize();
		rotatedRadians *= static_cast<float>(myWeaponData->randomSpreadAngle) / 2.0f * PI / 180.0f;
	}
	CU::Matrix44f rotatedMatrix = myUser->GetLocalTransform();
	rotatedMatrix.LookAt(rotatedMatrix.GetPosition() + aDirection);
	rotatedMatrix.Rotate(rotatedRadians.x, CU::Axees::Y);
	rotatedMatrix.Rotate(rotatedRadians.y, CU::Axees::X);
	rotatedMatrix.Move(CU::Vector3f(0.0f, 0.0f, 10.0f));
	CU::Matrix44f unRotatedMatrix = myUser->GetLocalTransform();
	unRotatedMatrix.LookAt(unRotatedMatrix.GetPosition() + aDirection);
	CU::Vector3f direction = rotatedMatrix.GetPosition() - unRotatedMatrix.GetPosition();
	return direction;

}

const SWeaponData* CWeapon::GetData() const
{
	return myWeaponData;
}

bool CWeapon::CanShoot()
{
	if (myElapsedFireTimer >= myWeaponData->fireRate)
	{
		return true;
	}
	return false;
}

volatile static int onlyOnce = 1;
void CWeapon::Equip()
{
	if (myWeaponObject != nullptr)
	{
		SComponentMessageData visibilityTrue;
		visibilityTrue.myBool = true;
		myWeaponObject->NotifyOnlyComponents(eComponentMessageType::eSetVisibility, visibilityTrue);

		myWeaponObject->NotifyOnlyComponents(eComponentMessageType::eEquip, SComponentMessageData());



		onlyOnce++; //Don't question it.
		if (myWeaponData->isMeleeWeapon == false)
			Audio::CAudioInterface::GetInstance()->PostEvent("Player_Chainsaw_Throttle_Stop");
		if (myWeaponData->isMeleeWeapon == true && onlyOnce == 3)
		{
			Audio::CAudioInterface::GetInstance()->PostEvent("Player_Chainsaw_Throttle_Start"); // gör possitionerat.
			onlyOnce = 1;
		}
	}
}

void CWeapon::Unequip(const std::function<void(void)>& aOnUnequippedCallback)
{
	if (myWeaponObject != nullptr)
	{
		SComponentMessageData visibilityFalse;
		visibilityFalse.myBool = false;
		myWeaponObject->NotifyOnlyComponents(eComponentMessageType::eSetVisibility, visibilityFalse);

		SComponentMessageData unequipData;
		unequipData.myVoidFunction = &aOnUnequippedCallback;
		myWeaponObject->NotifyOnlyComponents(eComponentMessageType::eUnequip, unequipData);
	}
}

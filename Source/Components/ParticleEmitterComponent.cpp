#include "stdafx.h"
#include "ParticleEmitterComponent.h"
#include "GameObject.h"
#include "ComponentManager.h"
#include "ParticleEmitterComponentManager.h"

CParticleEmitterComponent::CParticleEmitterComponent(CParticleEmitterInstance* aEmitterInstance)
{
	myParticleInstance = aEmitterInstance;
	myType = eComponentType::eParticleEmitter;
}


CParticleEmitterComponent::~CParticleEmitterComponent()
{
}

void CParticleEmitterComponent::Update(CU::Time aDeltaTime)
{	
	myParticleInstance->SetPosition(GetParent()->GetToWorldTransform().GetPosition());
	myParticleInstance->Update(aDeltaTime);
}

void CParticleEmitterComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent: break;
	case eComponentMessageType::ePlaySound: break;
	case eComponentMessageType::eMoving: break;
	case eComponentMessageType::eAddArmor: break;
	case eComponentMessageType::eSetVisibility: break;
	case eComponentMessageType::eSetIsColliderActive: break;
	case eComponentMessageType::eObjectDone: break;
	case eComponentMessageType::eAddMessageSubscription: break;
	case eComponentMessageType::eSetHighlight: break;
	case eComponentMessageType::eTurnOnThePointLight: break;
	case eComponentMessageType::eTurnOffThePointLight: break;
	case eComponentMessageType::eSetPointLightIntensity: break;
	case eComponentMessageType::eSetPointLightRange: break;
	case eComponentMessageType::eSetPointLightColor: break;
	case eComponentMessageType::eSetPointLightToLastState: break;
	case eComponentMessageType::eKeyPressed: break;
	case eComponentMessageType::eKeyReleased: break;
	case eComponentMessageType::ePitch: break;
	case eComponentMessageType::eTryToShoot: break;
	case eComponentMessageType::eCheckIfHaveAmmoForShooting: break;
	case eComponentMessageType::eShoot: break;
	case eComponentMessageType::eShootWithNetworking: break;
	case eComponentMessageType::eAddWeapon: break;
	case eComponentMessageType::eAddNewAmmoType: break;
	case eComponentMessageType::eWeaponFactoryGiveWeaponToWeaponSystem: break;
	case eComponentMessageType::eGiveAmmo: break;
	case eComponentMessageType::eChangeWeapon: break;
	case eComponentMessageType::eTakeDamage: break;
	case eComponentMessageType::eHeal: break;
	case eComponentMessageType::eTookDamage: break;
	case eComponentMessageType::eDied: break;
	case eComponentMessageType::eStopJumping: break;
	case eComponentMessageType::eSelectWeapon: break;
	case eComponentMessageType::eOnTriggerEnter: break;
	case eComponentMessageType::eOnTriggerExit: break;
	case eComponentMessageType::eOnCollisionEnter: break;
	case eComponentMessageType::eOnCollisionExit: break;
	case eComponentMessageType::eCheckPointReset: break;
	case eComponentMessageType::eSetControllerPosition: break;
	case eComponentMessageType::eActivateExplosion: break;
	case eComponentMessageType::eActivate: break;
	case eComponentMessageType::eDeactivate: break;
	case eComponentMessageType::eNetworkDoDamage: break;
	case eComponentMessageType::eLength: break;
	case eComponentMessageType::eActivateEmitter:
		Activate();
		break;
	default: break;
	}
}

void CParticleEmitterComponent::Destroy()
{
	CParticleEmitterComponentManager::GetInstance().Remove(this);
}

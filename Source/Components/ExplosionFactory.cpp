#include "stdafx.h"
#include "ExplosionFactory.h"
#include "GameObjectManager.h"
#include "ExplosionComponentManager.h"
#include "ModelComponentManager.h"
#include "ExplosionBufferData.h"
#include "../ThreadedPostmaster/CreateExplosionMessage.h"
#include "../ThreadedPostmaster/DeactivateExplosionMessage.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/SetAsNewCheckPointMessage.h"
#include "ExplosionComponent.h"
#include "ColliderComponentManager.h"
#include "ParticleEmitterComponentManager.h"

CExplosionFactory::CExplosionFactory(CExplosionComponentManager* aExplosionComponentManager)
{
	myPassiveExplosions.Init(10);
	myActiveExplosions.Init(10);
	myExplosionComponentsManager = aExplosionComponentManager;
}


CExplosionFactory::~CExplosionFactory()
{
	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
}

void CExplosionFactory::Init(CGameObjectManager* aGameObjectManager, CModelComponentManager* aModelComponentManagerPointer, CColliderComponentManager* aCColliderComponentManager)
{
	myGameObjectManagerPointer = aGameObjectManager;
	myModelComponentManagerPointer = aModelComponentManagerPointer;
	myColliderComponentManagerPointer = aCColliderComponentManager;

	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eCreateExplosionMessage);
	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eDeactivateExplosion);
}

eMessageReturn CExplosionFactory::DoEvent(const CCreateExplosionMessage& aCreateExplosionMessage)
{
	if(myPassiveExplosions.Size() > 0)
	{
		myActiveExplosions.Add(myPassiveExplosions[0]);
		myPassiveExplosions.RemoveCyclicAtIndex(0);
		SComponentMessageData EXuPlosssiioooooooonData;
		EXuPlosssiioooooooonData.myExplosionData = &aCreateExplosionMessage.GetExplosionData();
		myActiveExplosions.GetLast()->gameObject->NotifyComponents(eComponentMessageType::eActivateExplosion, EXuPlosssiioooooooonData);
		myActiveExplosions.GetLast()->gameObject->SetWorldPosition(aCreateExplosionMessage.GetPosition());
		myActiveExplosions.GetLast()->gameObject->NotifyOnlyComponents(eComponentMessageType::eMoving, SComponentMessageData());
		myActiveExplosions.GetLast()->gameObject->NotifyOnlyComponents(eComponentMessageType::eActivateEmitter, SComponentMessageData());
		SComponentMessageData visibilityData;
		visibilityData.myBool = true;
		myActiveExplosions.GetLast()->gameObject->NotifyComponents(eComponentMessageType::eSetVisibility, visibilityData);
	}
	else
	{
		CreateExplosion();
		DoEvent(aCreateExplosionMessage);
	}
	return eMessageReturn::eContinue;
}

eMessageReturn CExplosionFactory::DoEvent(const CDeactivateExplosionMessage& aDeactivateExplosionMessage)
{
	for(unsigned int i = 0; i < myActiveExplosions.Size(); i++)
	{
		if(myActiveExplosions[i]->gameObject == aDeactivateExplosionMessage.GetGameObject())
		{
			myPassiveExplosions.Add(myActiveExplosions[i]);
			myActiveExplosions.RemoveCyclicAtIndex(i);
			break;
		}
	}
	return eMessageReturn::eContinue;
}

void CExplosionFactory::CreateExplosion()
{
	CGameObject* newExplosionObject = myGameObjectManagerPointer->CreateGameObject();
	CExplosionComponent* tempExplosionComponent = myExplosionComponentsManager->CreateAndRegisterComponent();
	newExplosionObject->AddComponent(tempExplosionComponent);
	/*CModelComponent* modelComponent = myModelComponentManagerPointer->CreateComponent("Models/Meshes/M_Cactus_Large_03.fbx");
	newExplosionObject->AddComponent(modelComponent);*/
	newExplosionObject->NotifyOnlyComponents(eComponentMessageType::eMoving, SComponentMessageData());
	SComponentMessageData visibilityData;
	visibilityData.myBool = false;
	newExplosionObject->NotifyOnlyComponents(eComponentMessageType::eSetVisibility, visibilityData);

	SSphereColliderData sphereColliderDesc;
	sphereColliderDesc.myRadius = 6.0f;
	sphereColliderDesc.IsTrigger = false;

	unsigned int collideWith = Physics::CollideEverything;
	collideWith &= ~Physics::ECollisionLayer::eProjectile;
	sphereColliderDesc.myLayer = Physics::eProjectile;
	sphereColliderDesc.myCollideAgainst = static_cast<Physics::ECollisionLayer>(collideWith);
	sphereColliderDesc.IsTrigger = true;


	CColliderComponent* explosionSphereCollider = myColliderComponentManagerPointer->CreateComponent(&sphereColliderDesc);
	newExplosionObject->AddComponent(explosionSphereCollider);

	SEmitterData emitterData;
	emitterData.UseGravity = true;
	emitterData.Gravity = CU::Vector3f(0, -5, 0);
	emitterData.EmissionRate = 1000;
	emitterData.StartSize = 0.2;
	emitterData.EndSize = 0.5;
	emitterData.NumOfParticles = 1000;
	emitterData.MinParticleLifeTime = 2.5;
	emitterData.MaxParticleLifeTime = 5;
	const float bounds = 0;
	emitterData.MaxEmissionArea = (CU::Vector3f(bounds, bounds * 2.f, bounds));
	emitterData.MinEmissionArea = (CU::Vector3f(-bounds, 0.f, -bounds));
	const float vel = 7.5;
	const float nVel = vel / 10;
	emitterData.MinEmissionVelocity = CU::Vector3f(-nVel, vel / 2.f, -nVel);
	emitterData.MaxEmissionVelocity = CU::Vector3f(nVel, vel, nVel);
	emitterData.TexturePath = "Models/Textures/T_M_Rock_10m_RMA.dds";
	emitterData.StartColor = CU::Vector4f(0.8, 0.3, 0.1, 1);
	emitterData.EndColor = emitterData.StartColor;
	emitterData.EndColor.a = 0.f;
	emitterData.ColorCurve = eLerpCurve::eSmootherStep;
	emitterData.SizeCurve = eLerpCurve::eLinear;
	emitterData.RotationCurve = eLerpCurve::eSmootherStep;
	emitterData.ShouldLoop = false;
	emitterData.Lifetime = .25f;
	CParticleEmitterComponent* companent = CParticleEmitterComponentManager::GetInstance().CreateComponent(emitterData);
	newExplosionObject->AddComponent(companent);

	SExplosionBufferData* newSexplosionData = new SExplosionBufferData();
	newSexplosionData->gameObject = newExplosionObject;
	newSexplosionData->data = nullptr;
	myPassiveExplosions.Add(newSexplosionData);


}
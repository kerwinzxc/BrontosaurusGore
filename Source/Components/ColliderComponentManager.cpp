#include "stdafx.h"
#include "ColliderComponentManager.h"
#include "ComponentManager.h"
#include "Component.h"

#include "..\Physics\Physics.h"
#include "..\Physics\PhysicsScene.h"
#include "..\Physics\Shape.h"
#include "..\Physics\PhysicsActor.h"
#include "..\Physics\PhysicsActorDynamic.h"
#include "..\Physics\CharacterControllerManager.h"
#include "..\Physics\PhysicsCharacterController.h"
#include "CharcterControllerComponent.h"
#include "..\Physics\CollisionLayers.h"


CColliderComponentManager::CColliderComponentManager()
{
	myColliderComponents.Init(128);
	myControllerManager = nullptr;
}

CColliderComponentManager::~CColliderComponentManager()
{
	myColliderComponents.RemoveAll();

	if(myControllerManager)
		delete myControllerManager;
	myControllerManager= nullptr;
}

CColliderComponent* CColliderComponentManager::CreateComponent(SColliderData* aColliderData)
{
	switch (aColliderData->myType)
	{
	case SColliderData::eColliderType::eBox:
		myColliderComponents.Add(CreateBoxCollider(reinterpret_cast<SBoxColliderData*>(aColliderData)));
		break;
	case SColliderData::eColliderType::eSphere:

		break;
	case SColliderData::eColliderType::eCapsule:

		break;
	case SColliderData::eColliderType::eMesh:

		break;
	case SColliderData::eColliderType::eRigidbody:
		myColliderComponents.Add(CreateRigidbody(reinterpret_cast<SRigidBodyData*>(aColliderData)));
		break;
	default:
		break;
	}

	CComponentManager::GetInstance().RegisterComponent(myColliderComponents.GetLast());
	return myColliderComponents.GetLast();
}

void CColliderComponentManager::Update()
{
	for (CColliderComponent* collider : myColliderComponents)
	{
		if (collider->GetType() == SColliderData::eColliderType::eRigidbody)
		{
			collider->UpdatePosition();
		}
	}
}

void CColliderComponentManager::InitControllerManager()
{
	myControllerManager = new Physics::CCharacterControllerManager(myScene, myPhysics);
}

void CColliderComponentManager::RemoveActorFromScene(Physics::CPhysicsActor* aActor)
{
	myScene->RemoveActor(aActor);
}

CCharcterControllerComponent* CColliderComponentManager::CreateCharacterControllerComponent(const Physics::SCharacterControllerDesc& aParams)
{
	Physics::CPhysicsCharacterController* controller = myControllerManager->CreateCharacterController(aParams);
	CCharcterControllerComponent* component = new CCharcterControllerComponent(controller);
	CComponentManager::GetInstance().RegisterComponent(component);
	return component;
}

CColliderComponent* CColliderComponentManager::CreateBoxCollider(SBoxColliderData* aBoxColliderData)
{
	if (!aBoxColliderData)
	{
		DL_ASSERT("OH NO");
		return nullptr;
	}
	Physics::SMaterialData material;
	Physics::CShape* shape = myPhysics->CreateBoxShape(aBoxColliderData->myHalfExtent, material);
	if (!shape)
	{
		DL_ASSERT("OH NO");
		return nullptr;
	}

	shape->SetCollisionLayers(Physics::ECollisionLayer::eDefault);

	Physics::CPhysicsActor* actor = myPhysics->CreateStaticActor(shape, aBoxColliderData->IsTrigger);
	if (!actor)
	{
		DL_ASSERT("OH NO");
		return nullptr;
	}
	myScene->AddActor(actor);
	CBoxColliderComponent* component = new CBoxColliderComponent(aBoxColliderData, shape, actor);
	return component;
}

CColliderComponent* CColliderComponentManager::CreateRigidbody(class SRigidBodyData* aRigidbodyData)
{
	Physics::SMaterialData material;
	Physics::CShape* shape = nullptr;

	Physics::CPhysicsActorDynamic* actor = reinterpret_cast<Physics::CPhysicsActorDynamic*>(myPhysics->CreateDynamicActor(
		shape, 
		aRigidbodyData->IsTrigger,
		aRigidbodyData->mass, 
		aRigidbodyData->isKinematic, 
		aRigidbodyData->useGravity));

	actor->SetRotationLock(aRigidbodyData->freezedRotationAxiees);
	myScene->AddActor(actor);
	CRigidBodyComponent* component = new CRigidBodyComponent(aRigidbodyData, actor);
	component->SetManager(this);
	return component;
}

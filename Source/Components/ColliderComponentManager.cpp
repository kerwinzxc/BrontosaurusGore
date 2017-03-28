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
#include "../Physics/Collection.h"


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
		myColliderComponents.Add(CreateBoxCollider(*reinterpret_cast<SBoxColliderData*>(aColliderData)));
		break;
	case SColliderData::eColliderType::eSphere:
		myColliderComponents.Add(CreateSphereCollider(*reinterpret_cast<SSphereColliderData*>(aColliderData)));
		break;
	case SColliderData::eColliderType::eCapsule:
		myColliderComponents.Add(CreateCapsuleCollider(*reinterpret_cast<SCapsuleColliderData*>(aColliderData)));
		break;
	case SColliderData::eColliderType::eMesh:
		myColliderComponents.Add(CreateMeshCollider(*reinterpret_cast<SMeshColliderData*>(aColliderData)));
		break;
	case SColliderData::eColliderType::eRigidbody:
		myColliderComponents.Add(CreateRigidbody(*reinterpret_cast<SRigidBodyData*>(aColliderData)));
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
		collider->UpdateCallbacks();
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
	CCharcterControllerComponent* component = new CCharcterControllerComponent(controller, aParams.center);
	CComponentManager::GetInstance().RegisterComponent(component);
	return component;
}

CColliderComponent* CColliderComponentManager::CreateBoxCollider(const SBoxColliderData& aBoxColliderData)
{
	Physics::SMaterialData material;
	Physics::CShape* shape = myPhysics->CreateBoxShape(aBoxColliderData.myHalfExtent, material);
	
	if (!shape)
	{
		DL_ASSERT("Failed to create BoxShape");
		return nullptr;
	}

	shape->SetCollisionLayers(aBoxColliderData.myLayer, aBoxColliderData.myCollideAgainst);

	Physics::CPhysicsActor* actor = myPhysics->CreateStaticActor(shape, aBoxColliderData.IsTrigger);
	if (!actor)
	{
		DL_ASSERT("Failed to create physics actor");
		return nullptr;
	}
	myScene->AddActor(actor);
	CBoxColliderComponent* component = new CBoxColliderComponent(aBoxColliderData, shape, actor);
	return component;
}

CColliderComponent* CColliderComponentManager::CreateSphereCollider(const SSphereColliderData& aSphereColliderData)
{
	Physics::SMaterialData material;
	Physics::CShape* shape = myPhysics->CreateSphereShape(aSphereColliderData.myRadius, material);

	if (!shape)
	{
		DL_ASSERT("Failed to create BoxShape");
		return nullptr;
	}

	shape->SetCollisionLayers(aSphereColliderData.myLayer, aSphereColliderData.myCollideAgainst);

	Physics::CPhysicsActor* actor = myPhysics->CreateStaticActor(shape, aSphereColliderData.IsTrigger);
	if (!actor)
	{
		DL_ASSERT("Failed to create physics actor");
		return nullptr;
	}
	myScene->AddActor(actor);
	CSphereColliderComponent* component = new CSphereColliderComponent(aSphereColliderData, shape, actor);
	return component;
}

CColliderComponent* CColliderComponentManager::CreateCapsuleCollider(const SCapsuleColliderData& aCapsuleColliderData)
{
	Physics::SMaterialData material;
	Physics::CShape* shape = myPhysics->CreateCapsuleShape(aCapsuleColliderData.myRadius, aCapsuleColliderData.myHeight / 2.0f, material);

	if (!shape)
	{
		DL_ASSERT("Failed to create BoxShape");
		return nullptr;
	}

	shape->SetCollisionLayers(aCapsuleColliderData.myLayer, aCapsuleColliderData.myCollideAgainst);

	Physics::CPhysicsActor* actor = myPhysics->CreateStaticActor(shape, aCapsuleColliderData.IsTrigger);
	if (!actor)
	{
		DL_ASSERT("Failed to create physics actor");
		return nullptr;
	}
	myScene->AddActor(actor);
	CCapsuleColliderComponent* component = new CCapsuleColliderComponent(aCapsuleColliderData, shape, actor);
	return component;
}

CColliderComponent* CColliderComponentManager::CreateMeshCollider(const SMeshColliderData& aMeshColliderData)
{
	Physics::SMaterialData material;
	Physics::CShape* shape = myPhysics->CreateMeshShape(aMeshColliderData.myPath, material);

	if (!shape)
	{
		DL_ASSERT("Failed to create BoxShape");
		return nullptr;
	}

	shape->SetCollisionLayers(aMeshColliderData.myLayer, aMeshColliderData.myCollideAgainst);

	Physics::CPhysicsActor* actor = myPhysics->CreateStaticActor(shape, aMeshColliderData.IsTrigger);
	if (!actor)
	{
		DL_ASSERT("Failed to create physics actor");
		return nullptr;
	}
	myScene->AddActor(actor);
	CMeshColliderComponent* component = new CMeshColliderComponent(aMeshColliderData, shape, actor);
	return component;
}

CColliderComponent* CColliderComponentManager::CreateRigidbody(const SRigidBodyData& aRigidbodyData)
{
	Physics::SMaterialData material;
	Physics::CShape* shape = nullptr;

	Physics::CPhysicsActorDynamic* actor = reinterpret_cast<Physics::CPhysicsActorDynamic*>(myPhysics->CreateDynamicActor(
		shape, 
		aRigidbodyData.IsTrigger,
		aRigidbodyData.mass, 
		aRigidbodyData.isKinematic, 
		aRigidbodyData.useGravity));

	actor->SetRotationLock(aRigidbodyData.freezedRotationAxiees);
	myScene->AddActor(actor);
	CRigidBodyComponent* component = new CRigidBodyComponent(aRigidbodyData, actor);
	component->SetManager(this);
	return component;
}

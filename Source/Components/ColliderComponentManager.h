#pragma once

#include "BoxColliderComponent.h"
#include "SphereColliderComponent.h"
#include "CapsuleColliderComponent.h"
#include "MeshColliderComponent.h"
#include "RigidBodyComponent.h"

namespace Physics
{
	class CPhysicsScene;
	class CPhysics;
	class CCharacterControllerManager;
	class CCharacterControllerComponent;
	struct SCharacterControllerDesc;
}
class CCharacterControllerComponent;
class CColliderComponentManager
{

public:
	CColliderComponentManager();
	~CColliderComponentManager();
	CColliderComponent* CreateComponent(SColliderData* aColliderData, ComponentId anId);
	void Update();

	inline void SetPhysicsScene(Physics::CPhysicsScene* aScene) { myScene = aScene; }
	inline void SetPhysics(Physics::CPhysics* aPhysics) { myPhysics = aPhysics; }
	void InitControllerManager();
	void RemoveActorFromScene(Physics::CPhysicsActor* aActor);

	CCharacterControllerComponent* CreateCharacterControllerComponent(const Physics::SCharacterControllerDesc& aParams, ComponentId anId);

private:
	CColliderComponent* CreateBoxCollider(const SBoxColliderData& aBoxColliderData, ComponentId anId);
	CColliderComponent* CreateSphereCollider(const SSphereColliderData& aBoxColliderData, ComponentId anId);
	CColliderComponent* CreateCapsuleCollider(const SCapsuleColliderData& aBoxColliderData, ComponentId anId);
	CColliderComponent* CreateMeshCollider(const SMeshColliderData& aBoxColliderData, ComponentId anId);
	CColliderComponent* CreateRigidbody(const SRigidBodyData& aBoxColliderData, ComponentId anId);


private:
	CU::GrowingArray<CColliderComponent*> myColliderComponents;

	Physics::CCharacterControllerManager* myControllerManager;
	Physics:: CPhysicsScene* myScene;
	Physics::CPhysics* myPhysics;
};

#pragma once

#include "BoxColliderComponent.h"
#include "RigidBodyComponent.h"

namespace Physics
{
	class CPhysicsScene;
	class CPhysics;
	class CCharacterControllerManager;
	class CCharcterControllerComponent;
	struct SCharacterControllerDesc;
}
class CCharcterControllerComponent;
class CColliderComponentManager
{

public:
	CColliderComponentManager();
	~CColliderComponentManager();
	CColliderComponent* CreateComponent(SColliderData* aColliderData);
	void Update();

	inline void SetPhysicsScene(Physics::CPhysicsScene* aScene) { myScene = aScene; }
	inline void SetPhysics(Physics::CPhysics* aPhysics) { myPhysics = aPhysics; }
	void InitControllerManager();
	void RemoveActorFromScene(Physics::CPhysicsActor* aActor);

	CCharcterControllerComponent* CreateCharacterControllerComponent(const Physics::SCharacterControllerDesc& aParams);

private:
	CColliderComponent* CreateBoxCollider(SBoxColliderData* aBoxColliderData);
	CColliderComponent* CreateRigidbody(SRigidBodyData* aBoxColliderData);


private:
	CU::GrowingArray<CColliderComponent*> myColliderComponents;

	Physics::CCharacterControllerManager* myControllerManager;
	Physics:: CPhysicsScene* myScene;
	Physics::CPhysics* myPhysics;
};

#include "stdafx.h"
#include "LoadCollider.h"
#include "Component.h"
#include "BoxColliderComponent.h"
#include "ColliderComponent.h"
#include "ColliderComponentManager.h"
#include <ComponentManager.h>
#include <GameObject.h>
#include "RigidBodyComponent.h"
#include "Physics\PhysicsCharacterController.h"
#include "CharcterControllerComponent.h"

CGameObject* GetCurrentObject()
{
	int id = KLoader::CKevinLoader::GetInstance().GetCurrentObjectIndex();
	return reinterpret_cast<CGameObject*>(CComponentManager::GetInstance().GetComponent(id));
}

CColliderComponent* CreateComponent(SColliderData& aColData)
{
	CColliderComponentManager* colliderMan = LoadManager::GetInstance()->GetCurrentPLaystate().GetColliderComponentManager();
	return colliderMan->CreateComponent(&aColData);
}


CColliderComponent* CreateComponentServer(SColliderData& aColData)
{
	GET_SERVERLOADMANAGER(loadManager);
	CColliderComponentManager* colliderMan = loadManager.GetCurrentGameServer().GetColliderComponentManager();
	return colliderMan->CreateComponent(&aColData);
}

int LoadSphereCollider(KLoader::SLoadedComponentData someData)
{
	CGameObject* parent = GetCurrentObject();
	CU::Vector3f scale = parent->GetToWorldTransform().GetScale();
	CU::Vector3f parentPos = parent->GetToWorldTransform().GetPosition();

	SSphereColliderData data;
	data.IsTrigger = someData.myData.at("isTrigger").GetBool();
	data.material = someData.myData.at("material").GetVector3f("xyz");
	data.center = someData.myData.at("center").GetVector3f("xyz");
	data.center.x *= -1;
	data.center.z *= -1;
	data.center = data.center * parent->GetToWorldTransform().GetRotation();
	data.myRadius = someData.myData.at("radius").GetFloat() * scale.x;

	CColliderComponent* component = CreateComponent(data);
	return component->GetId();
}

int LoadCapsuleCollider(KLoader::SLoadedComponentData someData)
{
	CGameObject* parent = GetCurrentObject();
	CU::Vector3f scale = parent->GetToWorldTransform().GetScale();
	CU::Vector3f parentPos = parent->GetToWorldTransform().GetPosition();

	SCapsuleColliderData data;
	data.IsTrigger = someData.myData.at("isTrigger").GetBool();
	data.material = someData.myData.at("material").GetVector3f("xyz");
	data.center = someData.myData.at("center").GetVector3f("xyz");
	data.center.x *= -1;
	data.center.z *= -1;
	data.center = data.center * parent->GetToWorldTransform().GetRotation();	
	data.myRadius = someData.myData.at("radius").GetFloat() * scale.x;
	data.myHeight = someData.myData.at("height").GetFloat() * scale.x;

	CColliderComponent* component = CreateComponent(data);
	return component->GetId();
}

int LoadBoxCollider(KLoader::SLoadedComponentData someData)
{
	CGameObject* parent = GetCurrentObject();
	CU::Vector3f scale = parent->GetToWorldTransform().GetScale();
	CU::Vector3f parentPos = parent->GetToWorldTransform().GetPosition();

	SBoxColliderData data;
	data.IsTrigger = someData.myData.at("isTrigger").GetBool();
	data.material = someData.myData.at("material").GetVector3f("xyz");
	data.center = someData.myData.at("center").GetVector3f("xyz");
	data.center.x *= -1;
	data.center.z *= -1;
	data.center = data.center * parent->GetToWorldTransform().GetRotation();
	data.myHalfExtent = someData.myData.at("size").GetVector3f("xyz") * scale * 0.5f;

	CColliderComponent* component = CreateComponent(data);
	return component->GetId();
}

int LoadMeshCollider(KLoader::SLoadedComponentData someData)
{
	CGameObject* parent = GetCurrentObject();
	CU::Vector3f scale = parent->GetToWorldTransform().GetScale();
	CU::Vector3f parentPos = parent->GetToWorldTransform().GetPosition();

	SMeshColliderData data;
	data.IsTrigger = someData.myData.at("isTrigger").GetBool();
	data.material = someData.myData.at("material").GetVector3f("xyz");
	//data.center = someData.myData.at("center").GetVector3f("xyz");
	data.center.x *= -1;
	data.center.z *= -1;
	data.center = data.center * parent->GetToWorldTransform().GetRotation();
	data.myPath = someData.myData.at("meshPath").GetString().c_str();
	//data.myPath = "Models/PhysX/C_Rock_5m.xml";

	CColliderComponent* component = CreateComponent(data);
	return component->GetId();
}

int LoadRigidBody(KLoader::SLoadedComponentData someData)
{
	CGameObject* parent = GetCurrentObject();

	SRigidBodyData data;
	data.mass = someData.myData.at("mass").GetFloat();
	data.angularDrag = someData.myData.at("angularDrag").GetFloat();
	data.useGravity = someData.myData.at("useGravity").GetBool();
	data.isKinematic = someData.myData.at("isKinematic").GetBool();

	CColliderComponent* component = CreateComponent(data);
	return component->GetId();
}

int LoadCharacterController(KLoader::SLoadedComponentData someData)
{
	CGameObject* parent = GetCurrentObject();
	CColliderComponentManager* colliderMgr = LoadManager::GetInstance()->GetCurrentPLaystate().GetColliderComponentManager();
	Physics::SCharacterControllerDesc data;

	data.slopeLimit = someData.myData.at("slopeLimit").GetFloat();
	data.stepOffset = someData.myData.at("stepOffset").GetFloat();
	data.skinWidth = someData.myData.at("skinWidth").GetFloat();
	data.minMoveDistance = someData.myData.at("minMoveDistance").GetFloat();
	data.center = someData.myData.at("center").GetVector3f("xyz");
	data.radius = someData.myData.at("radius").GetFloat();
	data.halfHeight = someData.myData.at("height").GetFloat() / 2.0f;
	
	CCharcterControllerComponent* component = colliderMgr->CreateCharacterControllerComponent(data);
	return component->GetId();
}

int LoadSphereColliderServer(KLoader::SLoadedComponentData someData)
{
	CGameObject* parent = GetCurrentObject();
	CU::Vector3f scale = parent->GetToWorldTransform().GetScale();
	CU::Vector3f parentPos = parent->GetToWorldTransform().GetPosition();

	SSphereColliderData data;
	data.IsTrigger = someData.myData.at("isTrigger").GetBool();
	data.material = someData.myData.at("material").GetVector3f("xyz");
	data.center = someData.myData.at("center").GetVector3f("xyz");
	data.center.x *= -1;
	data.center.z *= -1;
	data.center = data.center * parent->GetToWorldTransform().GetRotation();
	data.myRadius = someData.myData.at("radius").GetFloat() * scale.x;

	CColliderComponent* component = CreateComponentServer(data);
	return component->GetId();
}

int LoadCapsuleColliderServer(KLoader::SLoadedComponentData someData)
{
	CGameObject* parent = GetCurrentObject();
	CU::Vector3f scale = parent->GetToWorldTransform().GetScale();
	CU::Vector3f parentPos = parent->GetToWorldTransform().GetPosition();

	SCapsuleColliderData data;
	data.IsTrigger = someData.myData.at("isTrigger").GetBool();
	data.material = someData.myData.at("material").GetVector3f("xyz");
	data.center = someData.myData.at("center").GetVector3f("xyz");
	data.center.x *= -1;
	data.center.z *= -1;
	data.center = data.center * parent->GetToWorldTransform().GetRotation();
	data.myRadius = someData.myData.at("radius").GetFloat() * scale.x;
	data.myHeight = someData.myData.at("height").GetFloat() * scale.x;

	CColliderComponent* component = CreateComponentServer(data);
	return component->GetId();
}

int LoadBoxColliderServer(KLoader::SLoadedComponentData someData)
{
	CGameObject* parent = GetCurrentObject();
	CU::Vector3f scale = parent->GetToWorldTransform().GetScale();
	CU::Vector3f parentPos = parent->GetToWorldTransform().GetPosition();

	SBoxColliderData data;
	data.IsTrigger = someData.myData.at("isTrigger").GetBool();
	data.material = someData.myData.at("material").GetVector3f("xyz");
	data.center = someData.myData.at("center").GetVector3f("xyz");
	data.center.x *= -1;
	data.center.z *= -1;
	data.center = data.center * parent->GetToWorldTransform().GetRotation();
	data.myHalfExtent = someData.myData.at("size").GetVector3f("xyz") * scale * 0.5f;

	CColliderComponent* component = CreateComponentServer(data);
	return component->GetId();
}

int LoadMeshColliderServer(KLoader::SLoadedComponentData someData)
{
	CGameObject* parent = GetCurrentObject();
	CU::Vector3f scale = parent->GetToWorldTransform().GetScale();
	CU::Vector3f parentPos = parent->GetToWorldTransform().GetPosition();

	SMeshColliderData data;
	data.IsTrigger = someData.myData.at("isTrigger").GetBool();
	data.material = someData.myData.at("material").GetVector3f("xyz");
	//data.center = someData.myData.at("center").GetVector3f("xyz");
	data.center.x *= -1;
	data.center.z *= -1;
	data.center = data.center * parent->GetToWorldTransform().GetRotation();
	//data.myPath = someData.myData.at("meshPath").GetString().c_str();
	data.myPath = "Models/PhysX/C_Rock_5m.xml";

	CColliderComponent* component = CreateComponentServer(data);
	return component->GetId();
}

int LoadRigidBodyServer(KLoader::SLoadedComponentData someData)
{
	CGameObject* parent = GetCurrentObject();

	SRigidBodyData data;
	data.mass = someData.myData.at("mass").GetFloat();
	data.angularDrag = someData.myData.at("angularDrag").GetFloat();
	data.useGravity = someData.myData.at("useGravity").GetBool();
	data.isKinematic = someData.myData.at("isKinematic").GetBool();

	CColliderComponent* component = CreateComponentServer(data);
	return component->GetId();
}

int LoadCharacterControllerServer(KLoader::SLoadedComponentData someData)
{
	CGameObject* parent = GetCurrentObject();
	GET_SERVERLOADMANAGER(loadManager);
	CColliderComponentManager* colliderMan = loadManager.GetCurrentGameServer().GetColliderComponentManager();
	Physics::SCharacterControllerDesc data;

	data.slopeLimit = someData.myData.at("slopeLimit").GetFloat();
	data.stepOffset = someData.myData.at("stepOffset").GetFloat();
	data.skinWidth = someData.myData.at("skinWidth").GetFloat();
	data.minMoveDistance = someData.myData.at("minMoveDistance").GetFloat();
	data.center = someData.myData.at("center").GetVector3f("xyz");
	data.radius = someData.myData.at("radius").GetFloat();
	data.halfHeight = someData.myData.at("height").GetFloat() / 2.0f;

	CCharcterControllerComponent* component = colliderMan->CreateCharacterControllerComponent(data);
	return component->GetId();
}


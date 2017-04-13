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
#include "CharacterControllerComponent.h"
#include "../Game/PollingStation.h"

#define TO_RADIANS(x) (x) * (3.1415f / 180.f)

CGameObject* GetCurrentObject()
{
	int id = KLoader::CKevinLoader::GetInstance().GetCurrentObjectIndex();
	return reinterpret_cast<CGameObject*>(CComponentManager::GetInstance().GetComponent(id));
}

CColliderComponent* CreateComponent(SColliderData& aColData)
{
	CGameObject* parent = GetCurrentObject();
	CColliderComponentManager* colliderMan = LoadManager::GetInstance()->GetCurrentPLaystate().GetColliderComponentManager();
	return colliderMan->CreateComponent(&aColData, parent->GetId());
}


CColliderComponent* CreateComponentServer(SColliderData& aColData)
{
	CGameObject* parent = GetCurrentObject();
	GET_SERVERLOADMANAGER(loadManager);
	CColliderComponentManager* colliderMan = loadManager.GetCurrentGameServer().GetColliderComponentManager();
	return colliderMan->CreateComponent(&aColData, parent->GetId());
}

int LoadSphereCollider(KLoader::SLoadedComponentData someData)
{
	CGameObject* parent = GetCurrentObject();
	CU::Vector3f scale = parent->GetToWorldTransform().GetScale();
	CU::Vector3f parentPos = parent->GetToWorldTransform().GetPosition();

	SSphereColliderData data;
	data.IsTrigger = someData.myData.at("isTrigger").GetBool();
	
	CU::Vector3f material = someData.myData.at("material").GetVector3f("xyz");
	data.material.aStaticFriction = material.x;
	data.material.aDynamicFriction = material.y;
	data.material.aRestitution = material.z;

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

	CU::Vector3f material = someData.myData.at("material").GetVector3f("xyz");
	data.material.aStaticFriction = material.x;
	data.material.aDynamicFriction = material.y;
	data.material.aRestitution = material.z;

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
	
	CU::Vector3f material = someData.myData.at("material").GetVector3f("xyz");
	data.material.aStaticFriction = material.x;
	data.material.aDynamicFriction = material.y;
	data.material.aRestitution = material.z;

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
	
	CU::Vector3f material = someData.myData.at("material").GetVector3f("xyz");
	data.material.aStaticFriction = material.x;
	data.material.aDynamicFriction = material.y;
	data.material.aRestitution = material.z;

	//data.center = someData.myData.at("center").GetVector3f("xyz");
	data.center.x *= -1;
	data.center.z *= -1;
	data.center = data.center * parent->GetToWorldTransform().GetRotation();
	data.myPath = someData.myData.at("meshPath").GetString().c_str();

	CColliderComponent* component = CreateComponent(data);
	if(component == nullptr)
	{
		return -1;
	}

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
	data.radius += 1.7f;
	float height = someData.myData.at("height").GetFloat();
	height += 0.7f;
	height /= 2.0f;
	data.halfHeight = height;
	
	CCharacterControllerComponent* component = colliderMgr->CreateCharacterControllerComponent(data, parent->GetId());
	return component->GetId();
}

int LoadSphereColliderServer(KLoader::SLoadedComponentData someData)
{
	CGameObject* parent = GetCurrentObject();
	CU::Vector3f scale = parent->GetToWorldTransform().GetScale();
	CU::Vector3f parentPos = parent->GetToWorldTransform().GetPosition();

	SSphereColliderData data;
	data.IsTrigger = someData.myData.at("isTrigger").GetBool();
	
	CU::Vector3f material = someData.myData.at("material").GetVector3f("xyz");
	data.material.aStaticFriction = material.x;
	data.material.aDynamicFriction = material.y;
	data.material.aRestitution = material.z;

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
	
	CU::Vector3f material = someData.myData.at("material").GetVector3f("xyz");
	data.material.aStaticFriction = material.x;
	data.material.aDynamicFriction = material.y;
	data.material.aRestitution = material.z;

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
	parent->GetLocalTransform().GetPosition().z += 20.0f;
	GET_SERVERLOADMANAGER(loadManager);
	short levelIndex = loadManager.GetCurrentGameServer().GetCurrentLevelIndex();

	SBoxColliderData data;
	data.IsTrigger = someData.myData.at("isTrigger").GetBool();
	
	CU::Vector3f material = someData.myData.at("material").GetVector3f("xyz");
	data.material.aStaticFriction = material.x;
	data.material.aDynamicFriction = material.y;
	data.material.aRestitution = material.z;

	data.center = someData.myData.at("center").GetVector3f("xyz");
	data.center.x *= -1;
	data.center.z *= -1;

	if(levelIndex < 3)
	{
		parent->GetLocalTransform().GetPosition().z -= 20.0f;
	}

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
	
	CU::Vector3f material = someData.myData.at("material").GetVector3f("xyz");
	data.material.aStaticFriction = material.x;
	data.material.aDynamicFriction = material.y;
	data.material.aRestitution = material.z;
	//data.center = someData.myData.at("center").GetVector3f("xyz");
	data.center.x *= -1;
	data.center.z *= -1;
	data.center = data.center * parent->GetToWorldTransform().GetRotation();
	data.myPath = someData.myData.at("meshPath").GetString().c_str();

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

	data.slopeLimit = TO_RADIANS(someData.myData.at("slopeLimit").GetFloat());
	data.stepOffset = someData.myData.at("stepOffset").GetFloat();
	data.skinWidth = someData.myData.at("skinWidth").GetFloat();
	data.minMoveDistance = someData.myData.at("minMoveDistance").GetFloat();
	data.center = someData.myData.at("center").GetVector3f("xyz");
	data.center.x *= -1;
	data.center.z *= -1; // ska vara med?
	data.center = data.center * parent->GetToWorldTransform().GetRotation();
	data.radius = someData.myData.at("radius").GetFloat();
	data.radius -= 0.5f;
	data.halfHeight = someData.myData.at("height").GetFloat() / 2.0f;

	CCharacterControllerComponent* component = colliderMan->CreateCharacterControllerComponent(data, parent->GetId());
	return component->GetId();
}


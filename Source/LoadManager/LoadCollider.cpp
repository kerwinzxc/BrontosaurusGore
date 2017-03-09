#include "stdafx.h"
#include "LoadCollider.h"
#include "Component.h"
#include "BoxColliderComponent.h"
#include "ColliderComponent.h"
#include "ColliderComponentManager.h"
#include <ComponentManager.h>
#include <GameObject.h>
#include "RigidBodyComponent.h"

int LoadBoxCollider(KLoader::SLoadedComponentData someData)
{
	int id = KLoader::CKevinLoader::GetInstance().GetCurrentObjectIndex();
	CGameObject* parent = reinterpret_cast<CGameObject*>(CComponentManager::GetInstance().GetComponent(id));
	CU::Vector3f scale = parent->GetToWorldTransform().GetScale();
	CU::Vector3f parentPos = parent->GetToWorldTransform().GetPosition();

	CColliderComponentManager* colliderMan = LoadManager::GetInstance()->GetCurrentPLaystate().GetColliderComponentManager();
	SBoxColliderData data;
	data.IsTrigger = someData.myData.at("isTrigger").GetBool();
	data.material = someData.myData.at("material").GetVector3f("xyz");
	data.center = someData.myData.at("center").GetVector3f("xyz");
	data.center.x *= -1;
	data.center.z *= -1;
	data.center = data.center * parent->GetToWorldTransform().GetRotation();
	data.myHalfExtent = someData.myData.at("size").GetVector3f("xyz") * scale * 0.5f;

	CColliderComponent* component = colliderMan->CreateComponent(&data);

	return component->GetId();
}

int LoadRigidBody(KLoader::SLoadedComponentData someData)
{
	int id = KLoader::CKevinLoader::GetInstance().GetCurrentObjectIndex();
	CGameObject* parent = reinterpret_cast<CGameObject*>(CComponentManager::GetInstance().GetComponent(id));
	CColliderComponentManager* colliderMan = LoadManager::GetInstance()->GetCurrentPLaystate().GetColliderComponentManager();
	SRigidBodyData data;
	data.mass = someData.myData.at("mass").GetFloat();
	data.angularDrag = someData.myData.at("angularDrag").GetFloat();
	data.useGravity = someData.myData.at("useGravity").GetBool();
	data.isKinematic = someData.myData.at("isKinematic").GetBool();
	CColliderComponent* component = colliderMan->CreateComponent(&data);
	return component->GetId();
}


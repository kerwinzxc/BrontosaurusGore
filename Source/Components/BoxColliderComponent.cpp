#include "stdafx.h"
#include "BoxColliderComponent.h"
#include "../Physics/PhysXManager.h"


CBoxColliderComponent::CBoxColliderComponent(const SBoxColliderData& aColliderData, Physics::CShape* aShape, Physics::CPhysicsActor* aActor, const bool aIsTrigger)
	: CColliderComponent(aColliderData, aShape, aActor, aIsTrigger)
{
}

CBoxColliderComponent::~CBoxColliderComponent()
{

}

void CBoxColliderComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eAddComponent:
		if (aMessageData.myComponent == this)
		{
			const int ParentId = GetParent()->GetId();
		}

		break; //else: fall through;
	}
	CColliderComponent::Receive(aMessageType, aMessageData);
}

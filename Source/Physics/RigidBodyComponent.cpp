#include "stdafx.h"
#include "RigidBodyComponent.h"

#include "Components/ComponentMessage.h"
#include "CommonUtilities/vector3.h"
#include "Components/GameObject.h"
#include "PhysicsManager.h"


CRigidBodyComponent::CRigidBodyComponent()
{
}


CRigidBodyComponent::~CRigidBodyComponent()
{
}

void CRigidBodyComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	if (aMessageType == eComponentMessageType::eCollision)
	{
		SComponentMessageData tempData;
		tempData.myComponent = this;
		aMessageData.myComponent->GetParent()->NotifyComponents(eComponentMessageType::eRigidBody, tempData);
		return;
	}

	if (aMessageType == eComponentMessageType::eSolidCollision)
	{
		const CU::Vector3f currentVelocity = myManager->GetArigidBody(myRigidBodyId).GetVeolocity();

		const CU::Vector3f thisPosition = GetParent()->GetToWorldTransform().GetPosition();
		const CU::Vector3f otherPosition = aMessageData.myComponent->GetParent()->GetToWorldTransform().GetPosition();

		if (thisPosition == otherPosition)
		{
			return;
		}

		const CU::Vector3f vectorToMe = thisPosition - otherPosition;
		const CU::Vector3f collisionNormal = vectorToMe.GetNormalized();
		const CU::Vector3f impulse = collisionNormal * -currentVelocity.Dot(collisionNormal);

		CGameObject* topParent = GetParent();
		while (topParent->GetParent() != nullptr)
		{
			topParent = topParent->GetParent();
		}

		topParent->GetLocalTransform().Move(impulse);
		myManager->GetArigidBody(myRigidBodyId).AddImpulse(impulse);
	}
}

void CRigidBodyComponent::Destroy()
{
	DL_ASSERT("destruction of component not implemented");
}

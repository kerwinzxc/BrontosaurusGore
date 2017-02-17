#include "stdafx.h"
#include "ProjectileComponent.h"
#include "ProjectileData.h"


ProjectileComponent::ProjectileComponent()
{
}


ProjectileComponent::~ProjectileComponent()
{
}

void ProjectileComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
}

void ProjectileComponent::Destroy()
{
}

void ProjectileComponent::Activate(ProjectileData* someData, const CU::Vector3f& aDirection, const CU::Vector3f& aPosition)
{
	myData = someData;
	myDirection = aDirection;
	GetParent()->GetLocalTransform().SetPosition(aPosition);
	GetParent()->GetLocalTransform().LookAt(GetParent()->GetLocalTransform().GetPosition() + aDirection);
}

void ProjectileComponent::Update(float aDeltaTime)
{
	GetParent()->GetLocalTransform().Move(CU::Vector3f(0.0f, 0.0f, myData->movementSpeed * aDeltaTime));
	GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
}
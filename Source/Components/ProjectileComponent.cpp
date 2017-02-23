#include "stdafx.h"
#include "ProjectileComponent.h"
#include "ProjectileData.h"


CProjectileComponent::CProjectileComponent()
{
	myIsActive = false;
}


CProjectileComponent::~CProjectileComponent()
{
}

void CProjectileComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
}

void CProjectileComponent::Destroy()
{
}

void CProjectileComponent::Activate(SProjectileData* someData, const CU::Vector3f& aDirection, const CU::Vector3f& aPosition)
{
	myData = someData;
	myDirection = aDirection;
	GetParent()->GetLocalTransform().SetPosition(aPosition);
	GetParent()->GetLocalTransform().LookAt(GetParent()->GetLocalTransform().GetPosition() + aDirection);
	myIsActive = true;
}

void CProjectileComponent::Update(float aDeltaTime)
{
	if(myIsActive == true)
	{
		GetParent()->GetLocalTransform().Move(CU::Vector3f(0.0f, 0.0f, myData->movementSpeed * aDeltaTime));
		GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
	}
}
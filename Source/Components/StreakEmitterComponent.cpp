#include "stdafx.h"
#include "StreakEmitterComponent.h"
#include "GameObject.h"

CStreakEmitterComponent::CStreakEmitterComponent()
	: myStreakEmitterInstance(nullptr)
{
	myType = eComponentType::eStreakEmitter;
}


CStreakEmitterComponent::~CStreakEmitterComponent()
{
	SAFE_DELETE(myStreakEmitterInstance);
}


void CStreakEmitterComponent::Init(CStreakEmitterInstance * aParticleEmitterInstance)
{
	myStreakEmitterInstance = aParticleEmitterInstance;
}

void CStreakEmitterComponent::Update(CU::Time aDeltaTime)
{
	myStreakEmitterInstance->SetPosition(GetParent()->GetToWorldTransform().GetPosition());
	myStreakEmitterInstance->Update(aDeltaTime);
}

void CStreakEmitterComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{

}

void CStreakEmitterComponent::Destroy()
{
	DL_ASSERT("Not Implemented");
}

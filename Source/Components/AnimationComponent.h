#pragma once
#include "Component.h"

class CModelComponent;

class CAnimationComponent : public CComponent
{
public:
	struct SAnimationState;
	CAnimationComponent(CModelComponent& aModelComponent);
	~CAnimationComponent();

	void Update(const CU::Time aDeltaTime);
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:
	std::map<std::string, SAnimationState> myAnimationStates;
	CU::GrowingArray<SAnimationState> myAnimationStack;

	CU::Vector2f myLastPosition;
	CModelComponent& myModelComponent;
};

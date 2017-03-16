#pragma once
#include "Component.h"

class CModelComponent;

class CAnimationComponent : public CComponent
{
public:
	CAnimationComponent(CModelComponent& aModelComponent);
	~CAnimationComponent();

	void Update(const CU::Time aDeltaTime);
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:
	struct SAnimationState;
	std::map<std::string, SAnimationState> myAnimationStates;
	CU::GrowingArray<std::string> myAnimationStack;

	CModelComponent& myModelComponent;
};

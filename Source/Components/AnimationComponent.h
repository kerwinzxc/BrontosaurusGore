#pragma once
#include "Component.h"

class CModelComponent;
enum class eAnimationState;

class CAnimationComponent : public CComponent
{
public:
	struct SAnimation;
	CAnimationComponent(CModelComponent& aModelComponent);
	~CAnimationComponent();

	void Update(const CU::Time aDeltaTime);
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	static void UpdateAnimations(const CU::Time aDeltaTime);

private:
	std::map<eAnimationState, SAnimation> myAnimationStates;
	CU::GrowingArray<SAnimation> myAnimationStack;

	CU::Vector2f myLastPosition;
	CModelComponent& myModelComponent;
	float myMainTimer;

	static CU::GrowingArray<CAnimationComponent*> ourAnimations;
};

#pragma once
#include "Component.h"

class CModelComponent;
enum class eAnimationState;

struct SAnimation
{
	eAnimationState myAnimationKey;
	eAnimationState myNextAnimationKey;
	float myAnimationBlend = 0.f;
	float myBlendTime = 0.f;
	float myLifeTime = FLT_MAX;
	float myCoolDownTime = -1.f;
	bool myIsLooping = true;
};

class CAnimationComponent : public CComponent
{
public:
	CAnimationComponent(CModelComponent& aModelComponent);
	~CAnimationComponent();

	void Update(const CU::Time aDeltaTime);
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;

	static void UpdateAnimations(const CU::Time aDeltaTime);

private:
	void PushAnimation(const SAnimation& aAnimation);

	std::function<void(void)> myOnFinnishedCallback;

	std::map<std::string, SAnimation> myAnimationStates;
	CU::GrowingArray<SAnimation> myAnimationStack;

	CU::Vector2f myLastPosition;
	CModelComponent& myModelComponent;

	static CU::GrowingArray<CAnimationComponent*> ourAnimations;
};

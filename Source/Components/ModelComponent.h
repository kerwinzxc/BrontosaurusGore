#pragma once
#include "Component.h"

class CModelInstance;
enum class eAnimationState;

class CModelComponent : public CComponent
{
public:
	CModelComponent(CModelInstance& aModel);
	CModelComponent(CModelInstance& aModel, const bool aIsDebugSphere);
	CModelComponent(const CModelComponent& aCopy) = delete;
	~CModelComponent();

	CU::Matrix44f GetToWorldTransform();

	void SetVisibility(const bool aVisibility);
	void FlipVisibility();
	void Receive(const eComponentMessageType, const SComponentMessageData&) override;
	inline CModelInstance& GetModelInstance() { return myModel; }

	void SetAnimation(const eAnimationState aAnimationKey);
	void SetNextAnimation(const eAnimationState aAnimationKey);
	void SetAnimationLerpValue(const float aLerpValue);
	void SetIgnoreDepth(bool aShouldIgnoreDepth);

private:
	void CreateAnimationComponent();

	CModelInstance& myModel;
};

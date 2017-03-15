#pragma once
#include "Component.h"

class CModelInstance;

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

private:
	void ChangeAnimation(const char* aAnimationKey);

	CModelInstance& myModel;
};


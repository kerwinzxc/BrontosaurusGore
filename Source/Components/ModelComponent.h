#pragma once
#include "Component.h"
#include "../CommonUtilities/matrix44.h"

class CModelInstance;

class CModelComponent : public CComponent
{
public:
	CModelComponent();
	CModelComponent(CModelInstance& aModel);
	CModelComponent(CModelInstance& aModel, const bool aIsDebugSphere);
	~CModelComponent();

	CU::Matrix44f GetToWorldTransform();

	void SetVisibility(const bool aVisibility);
	void FlipVisibility();
	void Receive(const eComponentMessageType, const SComponentMessageData&) override;
	void Destroy() override;
	CModelInstance* GetAndReleaseModel();
	inline CModelInstance* GetModelInst() { return myModel; }
private:
	void ChangeAnimation(const char* aAnimationKey);
	//void ChangeDirection(const CU::Vector2f& aDirection2D);

	CModelInstance* myModel; // mebe reference?
};


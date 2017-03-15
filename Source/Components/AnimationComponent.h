#pragma once
#include "Component.h"

class CModelComponent;

class CAnimationComponent : public CComponent
{
public:
	CAnimationComponent(CModelComponent& aModelComponent);
	~CAnimationComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:
	std::string myCurrentAnimation;
	CModelComponent& myModelComponent;
};

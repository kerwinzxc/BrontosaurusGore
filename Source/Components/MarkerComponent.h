#pragma once
#include "Component.h"
class CMarkerComponent : public CComponent
{
public:
	CMarkerComponent();
	~CMarkerComponent();
	void Update(float aDeltaTime);
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
private:
	bool myIsVisible;
};


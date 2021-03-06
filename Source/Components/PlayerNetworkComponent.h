#pragma once
#include "Component.h"
class CPlayerNetworkComponent: public CComponent
{
public:
	CPlayerNetworkComponent();
	~CPlayerNetworkComponent();

	void SetID(const unsigned aID);

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:
	CU::Vector3f myInterpolationPosition;

	float myInterpolationSpeed;

	unsigned short myID;
	bool myIsActive;
};


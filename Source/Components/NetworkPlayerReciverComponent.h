#pragma once
#include "../Components/Component.h"
#include "../CommonUtilities/vector3.h"
#include "../CommonUtilities/CUTime.h"

class CNetworkPlayerReciverComponent: public CComponent
{
public:
	CNetworkPlayerReciverComponent();
	~CNetworkPlayerReciverComponent();

	unsigned GetPlayerID();
	void SetPlayerID(const unsigned anID);

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	void Update(const CU::Time aDeltaTime);

	void SetInpolationPosition(const CU::Vector3f& aPosition);

private:
	CU::Vector3f myInterpolationPosition;

	float myInterpolationSpeed;

	unsigned myPlayerId;
};


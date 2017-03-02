#pragma once
#include "../TShared/Tshared_NetworkId.h"
#include "../Components/Component.h"

class CNetworkComponent: public CComponent
{
public:
	CNetworkComponent(NetworkId aNetworkId);
	~CNetworkComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;

private:
	const NetworkId myNetworkId;
};


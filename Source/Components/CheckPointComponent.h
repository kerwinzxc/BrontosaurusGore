#pragma once
#include "Component.h"
#include "../ThreadedPostmaster/Subscriber.h"
class CCheckPointComponent : public CComponent
{
public:
	CCheckPointComponent();
	~CCheckPointComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Destroy() override;
	void Update(float aDeltaTime);
};


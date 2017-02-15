#pragma once
#include "Component.h"
#include "../Audio/AudioInterface.h"


class CAudioSourceComponent : public CComponent
{
	friend class CAudioSourceComponentManager;

public:
	CAudioSourceComponent();
	~CAudioSourceComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Update();
	void PushEvent(const char* aEvent);
	//void PushEvent(aEventID); for event id, let wwise generate ID header.
	void Destroy() override;
private:
	Audio::GameObjectID myGameObjectID;
	Audio::CAudioInterface* myAudioInterface;
	bool myIsActive;
};


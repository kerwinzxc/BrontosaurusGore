#pragma once

#include "MessageType.h"

class Event;

class Message
{
public:
	Message(const eMessageType aMessageType, const Event& aEvent) : myMessageType(aMessageType), myEvent(aEvent) {}
	~Message() {}

	const Event& myEvent;
	const eMessageType myMessageType;
};

#pragma once
#include "PostMasterKyle.h"
#include "MessageKyle.h"

class Subscriber
{
public:
	Subscriber(){}

	virtual ~Subscriber();
	virtual void Receive(const Message& aMsg) = 0;
};

inline Subscriber::~Subscriber()
{
	myPM->UnsubscribeMe(this);
}

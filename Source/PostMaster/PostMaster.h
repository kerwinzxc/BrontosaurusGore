#pragma once

#include "MessageType.h"

#define POSTMASTER PostMaster::GetInstance()

class Event;
class Subscriber;
class Message;

class PostMaster
{
public:
	static void CreateInstance();
	static void DestroyInstance();
	static PostMaster& GetInstance();
	static PostMaster* GetInstancePtr();

	//void InsertSubscriber(Subscriber* aSubscriber, const eMessageType aMessageType);
	void Subscribe(Subscriber* aSubscriber, const eMessageType aMessageType, const int aPriority);
	void Subscribe(Subscriber* aSubscriber, const eMessageType aMessageType);
	void UnSubscribe(Subscriber* aSubscriber, const eMessageType aMessageType);

	void SendLetter(const Message& aMessage);
	void SendLetter(const eMessageType aMessageType, const Event& aEvent);

private:
	PostMaster();
	~PostMaster();

	CU::StaticArray<CU::GrowingArray<Subscriber*, int>, MessageTypeLength> mySubscribers;

	static PostMaster* ourInstance;
public:
	void UnSubscribeEveryWhere(Subscriber* aSubscriber);
};


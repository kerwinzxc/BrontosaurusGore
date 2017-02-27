#include "stdafx.h"
#include "PostMaster.h"
#include "Subscriber.h"
#include "Message.h"
#include "EMessageReturn.h"

PostMaster* PostMaster::ourInstance = nullptr;

void PostMaster::UnSubscribeEveryWhere(Subscriber* aSubscriber)
{
#ifndef _RETAIL_BUILD

	for (int messageType = 0; messageType < mySubscribers.Size(); ++messageType)
	{
		for (int subscriber = 0; subscriber < mySubscribers[messageType].Size(); ++subscriber)
		{
			if (mySubscribers[messageType][subscriber] == aSubscriber)
			{
				assert(false && "READ THIS! :) gå till call stacken, kolla vilken destruktor du var i före ~Subscriber(),\n och i den destructorn lägg till en vanlig UnSubscribe på alla meddelanden som den subscribe:ar på. Så slipper vi den här jävla n^2-loopen för varje subscriber som deletas men ligger kvar i post mastern");
				mySubscribers[messageType].RemoveAtIndex(subscriber);
			}
		}
	}

#else

	SUPRESS_UNUSED_WARNING(aSubscriber);

#endif // !_RETAIL_BUILD
}

void PostMaster::CreateInstance()
{
	if (ourInstance != nullptr)
	{
		assert(!"Post master should not be created twice");
	}
	
	ourInstance = new PostMaster();
}

void PostMaster::DestroyInstance()
{
	if (ourInstance == nullptr)
	{
		assert(!"Post master is not created (is NULL)");
	}

	SAFE_DELETE(ourInstance);
}

//PostMaster& PostMaster::GetInstance()
//{
//	if (ourInstance == nullptr)
//	{
//		assert(!"Post master is not created (is NULL)");
//	}
//
//	return *ourInstance;
//}
//
//PostMaster* PostMaster::GetInstancePtr()
//{
//	return ourInstance;
//}

void PostMaster::Subscribe(Subscriber* aSubscriber, const eMessageType aMessageType, const int aPriority)
{
	aSubscriber->SetPriority(aPriority);
	CU::GrowingArray<Subscriber*, int>& subscribers = mySubscribers[IntCast(aMessageType)];

	for (int i = 0; i < subscribers.Size(); ++i)
	{
		if (subscribers[i]->GetPriority() < aPriority)
		{
			subscribers.Insert(i, aSubscriber);
			return;
		}
	}

	subscribers.Add(aSubscriber);
}

void PostMaster::Subscribe(Subscriber* aSubscriber, const eMessageType aMessageType)
{
	mySubscribers[IntCast(aMessageType)].Add(aSubscriber);
}

void PostMaster::UnSubscribe(Subscriber* aSubscriber, const eMessageType aMessageType)
{
	mySubscribers[IntCast(aMessageType)].Remove(aSubscriber);
}

void PostMaster::SendLetter(const Message& aMessage)
{
	CU::GrowingArray<Subscriber*, int>& subscribers = mySubscribers[IntCast(aMessage.myMessageType)];

	for (Subscriber* subscriber : subscribers)
	{
		if (subscriber->Recieve(aMessage) == eMessageReturn::eStop)
		{
			break;
		}
	}
}

void PostMaster::SendLetter(const eMessageType aMessageType, const Event& aEvent)
{
	Message message(aMessageType, aEvent);
	SendLetter(message);
}

PostMaster::PostMaster()
{
	for (CU::GrowingArray<Subscriber*, int>& subscriber : mySubscribers)
	{
		subscriber.Init(16);
	}
}

PostMaster::~PostMaster()
{
}

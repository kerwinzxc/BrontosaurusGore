#include "stdafx.h"
#include "PostMasterKyle.h"
#include "SubscriberKyle.h"
#include <assert.h>
#include "CommonUtilities.h"

CUPostMaster* CUPostMaster::myPostMaster = nullptr;

//inits the Growing array for all messages
CUPostMaster::CUPostMaster()
{
	for (short i = 0; i < static_cast<short>(eMsg::SIZE); ++i)
	{
		mySubscribers[static_cast<eMsg>(i)].Init(8); 
	}
}

CUPostMaster::~CUPostMaster()
{
}

void CUPostMaster::Subscribe(const eMsg aMsgType, Subscriber* aSubscriber)
{
	mySubscribers[aMsgType].Add(aSubscriber);
}

void CUPostMaster::Unsubscribe(const eMsg aMsgType, Subscriber* aSubscriber)
{
	mySubscribers[aMsgType].RemoveCyclic(aSubscriber);
}

// Called in the subscribers destructor to automatically unsub from all messages.
void CUPostMaster::UnsubscribeMe(Subscriber* aSubscriber)
{
	for (auto i = mySubscribers.begin(); i != mySubscribers.end(); ++i)
	{
		if (i->second.Find(aSubscriber) != static_cast<unsigned short>(-1))
		{
			i->second.RemoveCyclic(aSubscriber);
		}
	}
}

void CUPostMaster::SendMsg(Message aMessage)
{
	for (unsigned short i = 0; i < mySubscribers[aMessage.GetMsgType()].Size(); ++i)
	{
		mySubscribers[aMessage.GetMsgType()][i]->Receive(aMessage);
	}
	aMessage.DeleteData();
}

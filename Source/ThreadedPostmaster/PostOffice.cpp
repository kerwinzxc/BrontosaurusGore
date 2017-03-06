#include "PostOffice.h"
#include "Message.h"
#include "Postmaster.h"


Postmaster::Threaded::CPostOffice::~CPostOffice()
{
	myMessageQueue.DeleteAll();
	while (myNarrowMessageQueue.IsEmpty() == false)
	{
		NarrowcastStruct temp = myNarrowMessageQueue.Pop();
		delete temp.message;
	}
}

void Postmaster::Threaded::CPostOffice::HandleMessages()
{
	if (myOutgoingQueue.IsEmpty() == false)
	{
		CPostmaster::GetInstance().HandleOutgoingBroadcast(myOutgoingQueue);
	}
	if(myOutgoingQueue.IsEmpty() == false)
	{
		CPostmaster::GetInstance().HandleOutgoingNarrowcast(myOutgoingNarrowQueue);
	}

	HandleBroadcastMessages();
	HandleNarrowcastMessages();
}



void Postmaster::Threaded::CPostOffice::Unsubscribe(ISubscriber* aSubscriber)
{
	UnsubscribeBroadcast(aSubscriber);
	UnsubscribeNarrowcast(aSubscriber);
}

void Postmaster::Threaded::CPostOffice::AppendMessages(Container::CLocklessQueue<Postmaster::Message::IMessage*>& aBufferQueue)
{
	myMessageQueue.Append(aBufferQueue);
}

void Postmaster::Threaded::CPostOffice::BroadcastGlobal(Message::IMessage* aMessage)
{
	myOutgoingQueue.Push(aMessage);
}

void Postmaster::Threaded::CPostOffice::NarrowcastGlobal(Message::IMessage* aMessage, IObject* aObject)
{
	NarrowcastStruct message;
	message.message = aMessage;
	message.sourceObject = aObject;

	myOutgoingNarrowQueue.Push(message);
}

void Postmaster::Threaded::CPostOffice::HandleBroadcastMessages()
{
	while (myMessageQueue.IsEmpty() == false)
	{
		Message::IMessage* message = myMessageQueue.Pop();
		const std::array<std::vector<ISubscriber*>, 1>::value_type& subscribers = mySubscribers[static_cast<unsigned>(message->GetType())];
		for (int i = 0; i < subscribers.size(); ++i)
		{
			message->DoEvent(*subscribers[i]);
		}
		delete message;
	}
}

void Postmaster::Threaded::CPostOffice::HandleNarrowcastMessages()
{
	while (myNarrowMessageQueue.IsEmpty() == false)
	{
		NarrowcastStruct nMessage = myNarrowMessageQueue.Pop();

		NarrowSubscriberList& objectList = myNarrowSubstribers[static_cast<unsigned>(nMessage.message->GetType())];

		NarrowSubscriberList::iterator subscriberList = objectList.find(nMessage.sourceObject);

		if(subscriberList != objectList.end())
		{
			SubscriberList list = subscriberList->second;
			for(SubscriberList::iterator it = list.begin(); it != list.end(); ++it)
			{
				ISubscriber* subscriber = *it;

				nMessage.message->DoEvent(*subscriber);
			}
		}

		delete nMessage.message;
	}
}

void Postmaster::Threaded::CPostOffice::UnsubscribeBroadcast(ISubscriber* aSubscriber)
{
	for (auto subscribers = mySubscribers.begin(); subscribers != mySubscribers.end(); )
	{
		bool hasErased = false;
		for (auto subscriber = subscribers->begin(); subscriber != subscribers->end(); ++subscriber)
		{
			
			if (*subscriber == aSubscriber)
			{
				subscribers->erase(subscriber);
				hasErased = true;
				break;
			}
		}
		if(hasErased == false)
		{
			++subscribers;
		}
	}
}

void Postmaster::Threaded::CPostOffice::UnsubscribeNarrowcast(ISubscriber* aSubscriber)
{
	for(NarrowSubscriberMessageList::iterator objectList = myNarrowSubstribers.begin(); objectList != myNarrowSubstribers.end(); ++objectList)
	{
		for(NarrowSubscriberList::iterator objectSubscriberList = objectList->begin(); objectSubscriberList != objectList->end(); )
		{
			SubscriberList& subscriberList = objectSubscriberList->second;
			bool hasErased = false;
			for(SubscriberList::iterator subscriber = subscriberList.begin(); subscriber != subscriberList.end(); ++subscriber)
			{
				if(*subscriber == aSubscriber)
				{
					subscriberList.erase(subscriber);
					hasErased = true;
					break;
				}
			}

			if(hasErased == false)
			{
				++objectSubscriberList;
			}
		}
	}
}

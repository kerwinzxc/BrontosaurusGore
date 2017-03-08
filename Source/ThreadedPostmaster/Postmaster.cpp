#include "Postmaster.h"
#include "Message.h"
#include "PostOffice.h"
#include "../CommonUtilities/GrowingArray.h"

Postmaster::Threaded::CPostmaster* Postmaster::Threaded::CPostmaster::ourInstance = nullptr;

Postmaster::Threaded::CPostOffice&  Postmaster::Threaded::CPostmaster::AddThreadOffice()
{
	std::thread::id threadId = std::this_thread::get_id();

	std::lock_guard<std::mutex> lock(myOfficeLock);
	std::map<std::thread::id, CPostOffice*>::iterator it = myOffices.find(threadId);

	if(it != myOffices.end())
	{
		return *it->second;
	}

	CPostOffice* newOffice = new CPostOffice();
	myOffices[threadId] = newOffice;

	return *newOffice;
}

void Postmaster::Threaded::CPostmaster::SetOfficeActive(bool anActive)
{
	std::thread::id threadId = std::this_thread::get_id();

	std::lock_guard<std::mutex> lock(myOfficeLock);
	std::map<std::thread::id, CPostOffice*>::iterator it = myOffices.find(threadId);

	if (it != myOffices.end())
	{
		it->second->SetActive(anActive);
	}
}

Postmaster::Threaded::CPostOffice& Postmaster::Threaded::CPostmaster::GetThreadOffice()
{
	std::thread::id threadId = std::this_thread::get_id();

	std::map<std::thread::id, CPostOffice*>::iterator it = myOffices.find(threadId);

	if (it == myOffices.end())
	{
		return AddThreadOffice();
	}

	return *it->second;
}

void Postmaster::Threaded::CPostmaster::Broadcast(Message::IMessage* aMessage)
{
	if (aMessage != nullptr)
	{
		GetThreadOffice().BroadcastGlobal(aMessage);
	}
}

void Postmaster::Threaded::CPostmaster::BroadcastLocal(Message::IMessage* aMessage)
{
	if (aMessage != nullptr)
	{
		GetThreadOffice().Broadcast(aMessage);
	}
}

void Postmaster::Threaded::CPostmaster::Narrowcast(Message::IMessage* aMessage, IObject* aSourceObject)
{
	//TODO: Handle narrowcast messages like breadcast messages;
	if(aMessage != nullptr)
	{
		GetThreadOffice().NarrowcastGlobal(aMessage, aSourceObject);
	}
	/*if(aMessage != nullptr)
	{
		NarrowcastStruct temp;
		temp.message = aMessage;
		temp.sourceObject = aSourceObject;

		myNarrowMessageQueue.Push(temp);
		myMessageWaitCondition.notify_one();
	}*/
}

void Postmaster::Threaded::CPostmaster::NarrowcastLocal(Message::IMessage* aMessage, IObject* aSourceObject)
{
	if(aMessage != nullptr)
	{
		GetThreadOffice().Narrowcast(aMessage, aSourceObject);
	}
}

void Postmaster::Threaded::CPostmaster::Subscribe(ISubscriber* aSubscriber, eMessageType aSubscriptionType)
{
	GetThreadOffice().Subscribe(aSubscriber, aSubscriptionType);
}

void Postmaster::Threaded::CPostmaster::Subscribe(ISubscriber* aSubscriber, IObject* aSourceObject, eMessageType aSubscriptionType)
{
	GetThreadOffice().Subscribe(aSubscriber, aSourceObject, aSubscriptionType);
}

void Postmaster::Threaded::CPostmaster::Unsubscribe(ISubscriber* aSubscriber)
{
	GetThreadOffice().Unsubscribe(aSubscriber);
}

void Postmaster::Threaded::CPostmaster::HandleOutgoingBroadcast(Container::CLocklessQueue<Message::IMessage*>& aLocklessQueue)
{
	CU::GrowingArray<Message::IMessage*> buffer;
	buffer.Init(aLocklessQueue.Size());
	while (aLocklessQueue.IsEmpty() == false)
	{
		buffer.Add(aLocklessQueue.Pop());
	}
	std::map<std::thread::id, CPostOffice*>::iterator it;
	for (it = myOffices.begin(); it != myOffices.end(); ++it)
	{
		if(it->second->GetIsActive() == true)
		{
			Container::CLocklessQueue<Message::IMessage*> bufferQueue;
			for (int i = 0; i < buffer.Size(); ++i)
			{
				Message::IMessage* message = buffer[i];
				bufferQueue.Push(message->Copy());
			}
			it->second->AppendMessages(bufferQueue);
		}
	}

	buffer.DeleteAll();
}

void Postmaster::Threaded::CPostmaster::HandleOutgoingNarrowcast(Container::CLocklessQueue<NarrowcastStruct>& aLocklessQueue)
{
	while (aLocklessQueue.IsEmpty() == false)
	{
		NarrowcastStruct nMessage = aLocklessQueue.Pop();

		std::map<std::thread::id, CPostOffice*>::iterator it;
		for (it = myOffices.begin(); it != myOffices.end(); ++it)
		{
			if(it->second->GetIsActive() == true)
			{
				myOfficeLock.lock();
				it->second->Narrowcast(nMessage.message->Copy(), nMessage.sourceObject);
				myOfficeLock.unlock();
			}
		}
		delete nMessage.message;
	}
}

Postmaster::Threaded::CPostmaster::CPostmaster()
{
}

Postmaster::Threaded::CPostmaster::~CPostmaster()
{
	for (auto it = myOffices.begin(); it != myOffices.end(); ++it)
	{
		delete it->second;
	}
}

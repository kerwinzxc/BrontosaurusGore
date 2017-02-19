#include "Postmaster.h"
#include "Message.h"
#include "PostOffice.h"

Postmaster::Threaded::CPostmaster* Postmaster::Threaded::CPostmaster::ourInstance = nullptr;


void Postmaster::Threaded::CPostmaster::WaitForMessages()
{
	std::unique_lock<std::mutex> lock(myMessageWaitLock);
	myMessageWaitCondition.wait(lock, [this] { return this->ShouldRun(); });
}

void Postmaster::Threaded::CPostmaster::Start()
{
	myThread = new std::thread(Postmaster::Threaded::CPostmaster::Run());
}

void Postmaster::Threaded::CPostmaster::HandleMessages()
{
	HandleBroadcastMessages();
	HandleNarrowcastMessages();
}

void Postmaster::Threaded::CPostmaster::StopThread()
{
	Stop();
	if(myThread != nullptr)
	{
		myThread->join();
		delete myThread;
		myThread = nullptr;
	}
}

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

void Postmaster::Threaded::CPostmaster::BroadcastLocal(Message::IMessage* aMessage)
{
	if (aMessage != nullptr)
	{
		GetThreadOffice().Broadcast(aMessage);
	}
}

void Postmaster::Threaded::CPostmaster::Narrowcast(Message::IMessage* aMessage, IObject* aSourceObject)
{
	if(aMessage != nullptr)
	{
		NarrowcastStruct temp;
		temp.message = aMessage;
		temp.sourceObject = aSourceObject;

		myNarrowMessageQueue.Push(temp);
		myMessageWaitCondition.notify_one();
	}
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

bool Postmaster::Threaded::CPostmaster::ShouldRun() const
{
	return myMessageQueue.IsEmpty() == false || myNarrowMessageQueue.IsEmpty() == false || myIsRunning == false;
}

void Postmaster::Threaded::CPostmaster::HandleBroadcastMessages()
{
	while (myMessageQueue.IsEmpty() == false)
	{
		Message::IMessage* message = myMessageQueue.Pop();
		std::map<std::thread::id, CPostOffice*>::iterator it;
		for (it = myOffices.begin(); it != myOffices.end(); ++it)
		{
			myOfficeLock.lock();
			it->second->Broadcast(message->Copy());
			myOfficeLock.unlock();
		}
		delete message;
	}
}

void Postmaster::Threaded::CPostmaster::HandleNarrowcastMessages()
{
	while (myNarrowMessageQueue.IsEmpty() == false)
	{

		NarrowcastStruct nMessage = myNarrowMessageQueue.Pop();

		/*while(myNarrowMessageQueue.TryPop(nMessage) == 
			Container::CLocklessQueue<NarrowcastStruct>::Error::Blocked)
		{
			
		}

		if(nMessage.message == nullptr)
		{
			return;;
		}*/

		std::map<std::thread::id, CPostOffice*>::iterator it;
		for (it = myOffices.begin(); it != myOffices.end(); ++it)
		{
			myOfficeLock.lock();
			it->second->Narrowcast(nMessage.message->Copy(), nMessage.sourceObject);
			myOfficeLock.unlock();
		}
		delete nMessage.message;
	}
}

Postmaster::Threaded::CPostmaster::~CPostmaster()
{
	myMessageQueue.DeleteAll();

	while (myNarrowMessageQueue.IsEmpty() == false)
	{
		NarrowcastStruct temp = myNarrowMessageQueue.Pop();
		delete temp.message;
	}

	for (auto it = myOffices.begin(); it != myOffices.end(); ++it)
	{
		delete it->second;
	}
}

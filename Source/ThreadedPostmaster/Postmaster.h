#pragma once
#include <map>
#include <thread>
#include "../Container/LocklessQueue.h"
#include "NarrowcastStruct.h"

enum class eMessageType;

namespace Postmaster
{
	class IObject;
	class ISubscriber;
	namespace Message
	{
		class IMessage;
	}
	namespace Threaded
	{
		class CPostOffice;
		class CPostmaster
		{
		public:
			static void Create();
			static void Destroy();
			static CPostmaster& GetInstance();

			void WaitForMessages();
			void Run();
			void HandleMessages();
			void Stop();

			CPostOffice& AddThreadOffice();
			CPostOffice& GetThreadOffice();

			//Broadcast message to all threads.
			void Broadcast(Message::IMessage* aMessage);
			//Broadcast message to this thread.
			void BroadcastLocal(Message::IMessage* aMessage);

			//Narrowcast message to all threads.
			void Narrowcast(Message::IMessage* aMessage, IObject* aSourceObject);
			//Narrowcast message to this thread.
			void NarrowcastLocal(Message::IMessage* aMessage, IObject* aSourceObject);

			
			void Subscribe(ISubscriber* aSubscriber, eMessageType aSubscriptionType);
			//Subscribe to messages from the given object
			void Subscribe(ISubscriber* aSubscriber, IObject* aSourceObject, eMessageType aSubscriptionType);
			//Unsubscribe from all your subscriptions
			void Unsubscribe(ISubscriber* aSubscriber);
			bool ShouldRun() const;
		protected:
			void HandleBroadcastMessages();
			void HandleNarrowcastMessages();

			static CPostmaster* ourInstance;

			std::map<std::thread::id, CPostOffice*> myOffices;
			Container::CLocklessQueue<Message::IMessage*> myMessageQueue;
			Container::CLocklessQueue<NarrowcastStruct> myNarrowMessageQueue;
			volatile bool myIsRunning;
			std::mutex myOfficeLock;
			std::mutex myMessageWaitLock;
			std::condition_variable myMessageWaitCondition;
		private:
			CPostmaster();
			~CPostmaster();
		};
	}
}

inline void Postmaster::Threaded::CPostmaster::Create()
{
	ourInstance = new CPostmaster;
}

inline void Postmaster::Threaded::CPostmaster::Destroy()
{
	delete ourInstance;
	ourInstance = nullptr;
}

inline Postmaster::Threaded::CPostmaster& Postmaster::Threaded::CPostmaster::GetInstance()
{
	if (ourInstance == nullptr)
	{
		Create();
	}

	return *ourInstance;
}

inline void Postmaster::Threaded::CPostmaster::Run()
{
	while (myIsRunning == true)
	{
		WaitForMessages();
		HandleMessages();
	}
}

inline void Postmaster::Threaded::CPostmaster::Broadcast(Message::IMessage* aMessage)
{
	if (aMessage != nullptr)
	{
		myMessageQueue.Push(aMessage);
		myMessageWaitCondition.notify_one();
	}
}

inline void Postmaster::Threaded::CPostmaster::Stop()
{
	myIsRunning = false;
	myMessageWaitCondition.notify_one();
}


inline Postmaster::Threaded::CPostmaster::CPostmaster() : myIsRunning(true)
{
}


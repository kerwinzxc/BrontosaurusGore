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
			friend class CPostOffice;
			static void Create();
			static void Destroy();
			static CPostmaster& GetInstance();
			
			//Create a local post office (for that thread)
			CPostOffice& AddThreadOffice();

			//Create a local post office (for that thread)
			CPostOffice& AddThreadOffice(std::thread::id aId);

			//Set office active
			void SetOfficeActive(bool anActive);

			//Get the post office (for that thread)
			CPostOffice& GetThreadOffice();

			//Get the post office (for the specified thread)
			CPostOffice& GetThreadOffice(std::thread::id anId);

			//Broadcast message to all threads.
			void Broadcast(Message::IMessage* aMessage);
			//Broadcast message to this thread.
			void BroadcastLocal(Message::IMessage* aMessage);

			//Narrowcast message to all threads.
			void Narrowcast(Message::IMessage* aMessage, IObject* aSourceObject);
			//Narrowcast message to this thread.
			void NarrowcastLocal(Message::IMessage* aMessage, IObject* aSourceObject);

			//Subscriptions will only be valid on the thread they subscribe from.
			void Subscribe(ISubscriber* aSubscriber, eMessageType aSubscriptionType);
			//Allows you to specify a thread where the subscriber will be subscribed
			void Subscribe(ISubscriber* aSubscriber, eMessageType aSubscriptionType, std::thread::id anId);
			//Subscribe to messages from the given object. Subscriptions will only be valid on the thread they subscribe from.
			void Subscribe(ISubscriber* aSubscriber, IObject* aSourceObject, eMessageType aSubscriptionType);
			//Subscribe to messages from the given object. Subscriptions will only be valid on the specified thread.
			void Subscribe(ISubscriber* aSubscriber, IObject* aSourceObject, eMessageType aSubscriptionType, std::thread::id anId);
			//Unsubscribe from all your subscriptions
			void Unsubscribe(ISubscriber* aSubscriber);
			
		protected:
			void HandleOutgoingBroadcast(Container::CLocklessQueue<Message::IMessage*>& aLocklessQueue);
			void HandleOutgoingNarrowcast(Container::CLocklessQueue<NarrowcastStruct>& aLocklessQueue);
			
			static CPostmaster* ourInstance;

			std::map<std::thread::id, CPostOffice*> myOffices;
			std::mutex myOfficeLock;
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


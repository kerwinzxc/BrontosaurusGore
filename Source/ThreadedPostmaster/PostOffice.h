#pragma once
#include <vector>
#include <array>
#include "../PostMaster/MessageType.h"
#include "../CommonUtilities/LocklessQueue.h"
#include "NarrowcastStruct.h"
#include <map>

namespace Postmaster
{
	class IObject;

	namespace Message
	{
		class IMessage;
	}
	class ISubscriber;
	
	namespace Threaded
	{
		class CPostOffice
		{
		public:
			friend class CPostmaster;


			void HandleMessages();

			void Subscribe(ISubscriber* aSubscriber, eMessageType aType);
			void Subscribe(ISubscriber* aSubscriber, IObject* aSourceObject, eMessageType aSubscriptionType);
			void Unsubscribe(ISubscriber* aSubscriber);
			bool GetIsActive();
		protected:
			void SetActive(bool anActive);
			
			void AppendMessages(Container::CLocklessQueue<Message::IMessage*>& aBufferQueue);

			void BroadcastGlobal(Message::IMessage* aMessage);
			void Broadcast(Message::IMessage* aMessage);

			void NarrowcastGlobal(Message::IMessage* aMessage, IObject* aObject);
			void Narrowcast(Message::IMessage* aMessage, IObject* aSourceObject);

			void HandleBroadcastMessages();
			void HandleNarrowcastMessages();

			void UnsubscribeBroadcast(ISubscriber* aSubscriber);
			void UnsubscribeNarrowcast(ISubscriber* aSubscriber);

			Container::CLocklessQueue<Message::IMessage*> myOutgoingQueue;
			Container::CLocklessQueue<NarrowcastStruct> myOutgoingNarrowQueue;

			Container::CLocklessQueue<Message::IMessage*> myMessageQueue;
			Container::CLocklessQueue<NarrowcastStruct> myNarrowMessageQueue;
			std::atomic<bool> myIsActive;
			typedef std::vector<ISubscriber*> SubscriberList;
			typedef std::map<IObject*, SubscriberList> NarrowSubscriberList;
			typedef std::array<SubscriberList, static_cast<unsigned>(eMessageType::eLength)> SubscriberMessageList;
			typedef std::array<NarrowSubscriberList, static_cast<unsigned>(eMessageType::eLength)> NarrowSubscriberMessageList;
			SubscriberMessageList mySubscribers;
			NarrowSubscriberMessageList myNarrowSubstribers;

		private:
			CPostOffice();
			~CPostOffice();
		};
	}
}

inline Postmaster::Threaded::CPostOffice::CPostOffice() : myIsActive(true)
{
}

inline void Postmaster::Threaded::CPostOffice::Subscribe(ISubscriber* aSubscriber, eMessageType aType)
{
	mySubscribers[static_cast<unsigned>(aType)].push_back(aSubscriber);
}

inline void Postmaster::Threaded::CPostOffice::Subscribe(ISubscriber* aSubscriber, IObject* aSourceObject, eMessageType aSubscriptionType)
{
	myNarrowSubstribers[static_cast<unsigned>(aSubscriptionType)][aSourceObject].push_back(aSubscriber);
}



inline void Postmaster::Threaded::CPostOffice::Narrowcast(Message::IMessage* aMessage, IObject* aSourceObject)
{
	if(aMessage != nullptr)
	{
		NarrowcastStruct temp;
		temp.message = aMessage;
		temp.sourceObject = aSourceObject;
		myNarrowMessageQueue.Push(temp);
	}
}

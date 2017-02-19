#pragma once
#include "MessageTypes.h"

namespace Postmaster
{
	class ISubscriber;
	namespace Message
	{
		class IMessage
		{
		public:
			explicit IMessage(eMessageType aType);
			virtual ~IMessage();

			virtual IMessage* Copy() = 0;
			virtual void DoMessage(::Postmaster::ISubscriber& aSubscriber) = 0;
			eMessageType GetType() const;
		private:
			eMessageType myType;
		};
	}
}

inline Postmaster::Message::IMessage::IMessage(eMessageType aType) : myType(aType)
{
}

inline Postmaster::Message::IMessage::~IMessage()
{
}

inline eMessageType Postmaster::Message::IMessage::GetType() const
{
	return myType;
}

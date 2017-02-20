#pragma once
#include "../PostMaster/EMessageReturn.h"
enum class eMessageType;

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

			virtual eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const = 0;
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

inline Postmaster::Message::IMessage* Postmaster::Message::IMessage::Copy()
{
	return nullptr;
}

inline eMessageType Postmaster::Message::IMessage::GetType() const
{
	return myType;
}

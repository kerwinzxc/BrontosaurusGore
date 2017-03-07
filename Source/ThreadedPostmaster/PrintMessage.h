#pragma once
#include <string>
#include "Message.h"

namespace Postmaster
{
	namespace Message
	{
		class CPrintMessage : public IMessage
		{
		public:
			CPrintMessage(const std::string& aText);
			~CPrintMessage();
			const std::string& GetText() const;
			IMessage* Copy() override;
			eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;
		protected:
			std::string myText;
		};
	}
}


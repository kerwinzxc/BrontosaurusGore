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
			IMessage* Copy() override;
			const std::string& GetText() const;
			void DoMessage(::Postmaster::ISubscriber& aSubscriber) override;
		protected:
			std::string myText;
		};
	}
}

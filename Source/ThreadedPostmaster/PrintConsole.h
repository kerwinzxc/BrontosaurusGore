#pragma once
#include "Subscriber.h"
#include "Object.h"

namespace Postmaster
{
	class CPrintConsole : public ISubscriber, public IObject
	{
	public:
		CPrintConsole();
		~CPrintConsole();
		void DoMessage(const Message::CPrintMessage& aPrintMessage) override;
	};
}


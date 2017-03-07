#pragma once
#include "../ThreadedPostmaster/Subscriber.h"

namespace CommonUtilities
{
	class CConsolePrinter : Postmaster::ISubscriber
	{
	public:
		static void Create();
		static void Destroy();
		static CConsolePrinter& GetInstance();

		eMessageReturn DoEvent(const Postmaster::Message::CPrintMessage& aPrintMessage) override;
	protected:
		static CConsolePrinter* ourInstance;
		CConsolePrinter();
		~CConsolePrinter();
	
	};
}


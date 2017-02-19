#pragma once

namespace Postmaster
{
	namespace Message
	{
		class CPrintMessage;
	}

	class ISubscriber
	{
	public:
		ISubscriber();
		virtual ~ISubscriber();

		virtual void DoMessage(const Message::CPrintMessage& aPrintMessage);
	};
}


#pragma once

namespace Postmaster
{
	class IObject;
	namespace Message
	{
		class IMessage;
	}
	struct NarrowcastStruct
	{
		Message::IMessage* message = nullptr;
		IObject* sourceObject = nullptr;
	};
}

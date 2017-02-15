#include "stdafx.h"

#ifdef DONT_COMPILE_THIS_FOR_NOW
#include "Subscriber.h"
#include "vector2.h"

class Class : Subscriber
{
public:
	void Function()
	{
		myPM->Subscribe(eMsg::eCoolMessage, this);

		CU::Vector2i coolVector = {13,37};
		myPM->SendMsg(Message(eMsg::eCoolMessage, (void*) new CU::Vector2i(coolVector), true));
	}

	void Receive(const Message& aMsg) override
	{
		if (aMsg.GetMsgType() == eMsg::eCoolMessage)
		{
			CU::Vector2i CouldBeACharactersPositionOrSomethingCool;

														// Lite speciell syntax, men det är bara detta.
			CouldBeACharactersPositionOrSomethingCool = *(reinterpret_cast<CU::Vector2i*>(aMsg.GetData()));
		}
	}
};
#endif
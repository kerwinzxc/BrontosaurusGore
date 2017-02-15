#pragma once
#include <unordered_map>

//Sweet ass defines, every singleton should have it!
#define myPM CUPostMaster::GetInstance()

enum class eMsg
{
	eCoolMessage,
	SIZE
};

class Subscriber;
class Message;

class CUPostMaster
{
public:
	friend Subscriber;

	static CUPostMaster* const GetInstance()
	{
		if (myPostMaster == nullptr)
		{
			myPostMaster = new CUPostMaster();
		}
		return myPostMaster;
	}

	void Subscribe(const eMsg aMsgType, Subscriber* aSubscriber);
	void Unsubscribe(const eMsg aMsgType, Subscriber* aSubscriber);
	void SendMsg(Message aMessage);

private:
	void UnsubscribeMe(Subscriber* aSubscriber);
	CUPostMaster();
	~CUPostMaster();

	static CUPostMaster* myPostMaster;
	std::unordered_map<eMsg, CU::GrowingArray<Subscriber*> > mySubscribers;

};


#pragma once
#include <map>
#include <thread>
#include "../CommonUtilities/DL_Debug.h"
#include <mutex>

#define NO_ID static_cast<unsigned char>(-1)

class CNetworkMessage;

class CMessageStorage
{
public:
	static void CreateInstance();
	static CMessageStorage* GetInstance();
	static void DestroyInstance();

	template<typename MESSAGE_TYPE>
	bool CheckTypeExist() const;

	template<typename MESSAGE_TYPE>
	void AddType(CNetworkMessage* aNetworkMessage);

	template<typename MESSAGE_TYPE>
	CNetworkMessage* GetType();

private:
	static CMessageStorage* ourInstance;

	CMessageStorage();
	~CMessageStorage();

	template<typename MESSAGE_TYPE>
	struct SMessageId
	{
		static unsigned char ourId;
	};

	struct SMessageGuard
	{
		SMessageGuard() : myNetworkMessage(nullptr), myIsLocked(false)
		{
		}

		SMessageGuard(CNetworkMessage* aNetworkMessage)
		{
			myNetworkMessage = aNetworkMessage;
			myIsLocked = false;
		}

		CNetworkMessage* myNetworkMessage;
		bool myIsLocked;
	};

	unsigned char myCurrentFreeId;
	std::map<std::thread::id, std::map<unsigned char, SMessageGuard>> myPreCreatedMessages;

	//std::mutex myMutex;
};

template<typename MESSAGE_TYPE>
unsigned char CMessageStorage::SMessageId<MESSAGE_TYPE>::ourId = NO_ID;

template <typename MESSAGE_TYPE>
bool CMessageStorage::CheckTypeExist()const
{
	if (SMessageId<MESSAGE_TYPE>::ourId == NO_ID)
	{
		return false;
	}

	if (myPreCreatedMessages.count(std::this_thread::get_id()) == 0)
	{
		return false;
	}

	const std::map<unsigned char, SMessageGuard> & messages = myPreCreatedMessages.at(std::this_thread::get_id());

	if (messages.count(SMessageId<MESSAGE_TYPE>::ourId) == 0)
	{
		return false;
	}

	return true;
}

template <typename MESSAGE_TYPE>
void CMessageStorage::AddType(CNetworkMessage* aNetworkMessage)
{
	//myMutex.lock();
	if (SMessageId<MESSAGE_TYPE>::ourId == NO_ID)
	{
		SMessageId<MESSAGE_TYPE>::ourId = myCurrentFreeId;
		myCurrentFreeId += 1;
	}

	std::map<unsigned char, SMessageGuard> &thisThreadsMessages = myPreCreatedMessages[std::this_thread::get_id()];

	if (thisThreadsMessages.count(SMessageId<MESSAGE_TYPE>::ourId)  >= 1)
	{
		DL_ASSERT("Message type already added");
	}

	thisThreadsMessages[SMessageId<MESSAGE_TYPE>::ourId].myNetworkMessage = aNetworkMessage;
	//myMutex.unlock();
}

template <typename MESSAGE_TYPE>
CNetworkMessage* CMessageStorage::GetType()
{
	//myMutex.lock();
	if (CheckTypeExist<MESSAGE_TYPE>() == false)
	{
		DL_ASSERT("MessageType has not been created");
	}
	//myMutex.unlock();

	return myPreCreatedMessages.at(std::this_thread::get_id()).at(SMessageId<MESSAGE_TYPE>::ourId).myNetworkMessage;;
}

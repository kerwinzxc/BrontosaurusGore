#pragma once
//#include "../TServer/TServer_NetworkWrapper.h"

#include "NetworkMessage.h"
#include "MessageStorage.h"

#define NO_ID static_cast<unsigned char>(0)


struct SNetworkMessageHolder;

class CMessageManager
{
public:
	CMessageManager();
	~CMessageManager();

	template <class MESSAGE_TYPE>
	MESSAGE_TYPE* CreateMessage(const SNetworkPackageHeader& aHeader);

	CNetworkMessage* CreateMessage(const SNetworkMessageHolder& aMessageHolder);
	CNetworkMessage* CreateMessage(const SNetworkPackageHeader& aHeader);

private:
	std::mutex myMutex;
};

template <typename MESSAGE_TYPE>
MESSAGE_TYPE* CMessageManager::CreateMessage(const SNetworkPackageHeader &aHeader)
{
	myMutex.lock();
	if (CMessageStorage::GetInstance()->CheckTypeExist<MESSAGE_TYPE>() == false)
	{
		CMessageStorage::GetInstance()->AddType<MESSAGE_TYPE>(new MESSAGE_TYPE);
	}

	
	CNetworkMessage* message = CMessageStorage::GetInstance()->GetType<MESSAGE_TYPE>();
	message->ClearStream();
	myMutex.unlock();

	message->SetHeader(aHeader);

	return static_cast<MESSAGE_TYPE*>(message);
}

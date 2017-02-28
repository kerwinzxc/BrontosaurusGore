#pragma once
#include "../TServer/TServer_NetworkWrapper.h"
#include "../CommonUtilities/DL_Debug.h"

class CMessageManager
{
public:
	CMessageManager();
	~CMessageManager();

	template<typename TYPE>
	TYPE* CreateMessage(SNetworkPackageHeader aHeader);
private:

	
};

template <typename TYPE>
TYPE* CMessageManager::CreateMessage(SNetworkPackageHeader aHeader)
{
	CNetworkMessage* message = new TYPE;
	aHeader.myTimeStamp = GetCurrentTime();
	message->SetHeader(aHeader);

	if (message->GetHeader().myPackageType == 0)
	{
		DL_ASSERT("Package type bacame 0, should not do that sir");
	}

	return static_cast<TYPE*>(message);
}

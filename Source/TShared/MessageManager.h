#pragma once
#include "../TServer/TServer_NetworkWrapper.h"

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
	message->SetHeader(aHeader);
	return static_cast<TYPE*>(message);
}

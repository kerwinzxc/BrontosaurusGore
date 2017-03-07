#pragma once
#include "CommonNetworkIncludes.h"
#include <functional>
#include "ImportantNetworkMessage.h"
#include <map>

#define DEFAULT_PORT 27015
#define SERVER_PORT "27015"
#define MAX_PACKET_SIZE 512


//dynamic id defines
enum ePreDefId: short
{
	ID_ALL = 0,
	ID_ALL_BUT_ME,
	ID_SERVER,
	ID_FREE//must be last
};

typedef __int16 ClientID;

typedef std::function<void(SNetworkPackageHeader, const char*, unsigned int, const char*, const char*)> NetworkMessageRecieveFunction;// (header, data, sender ip, sender socket)

class TShared_NetworkWrapper
{
public:
	TShared_NetworkWrapper();
	~TShared_NetworkWrapper();

	bool CheckIfImportantMessageDone(__int16 aMessageID);
	bool Init(unsigned short aPort, CMessageManager* aMessageManager);
	__int16 Send(CNetworkMessage* aNetworkMessage, const char* aRecieverAdress, const char* aRecieverPort);

	CNetworkMessage* Recieve(char** senderIp = nullptr, char** senderPort = nullptr);


private:
	__int16 GetMessageCount();
	
	bool Send(SNetworkPackageHeader aPackageHeader, const char* aData, unsigned aDataSize, const char* aRecieverAdress, const char* aRecieverPort);
	char* myBuffer;
	unsigned int myCurrentBufferSize;

	std::map<__int16, CImportantNetworkMessage*> myImportantMessages;

	WSAData myWsaData;
	SOCKET mySocket;
	bool myShallClose;
	__int16 myMessageCount;
	 CMessageManager* myMessageManager;
};


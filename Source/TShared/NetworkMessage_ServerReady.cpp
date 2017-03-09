#include "stdafx.h"
#include "NetworkMessage_ServerReady.h"


CNetworkMessage_ServerReady::CNetworkMessage_ServerReady()
{
}


CNetworkMessage_ServerReady::~CNetworkMessage_ServerReady()
{
}

ePackageType CNetworkMessage_ServerReady::GetPackageType() const
{
	return ePackageType::eServerReady;
}

unsigned CNetworkMessage_ServerReady::GetNumberOfPlayers()
{
	return myNumberOfPlayersToWaitFor;
}

void CNetworkMessage_ServerReady::SetNumberOfPlayers(unsigned aNumberOfPlayers)
{
	myNumberOfPlayersToWaitFor = aNumberOfPlayers;
}

void CNetworkMessage_ServerReady::DoSerialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoSerialize(aStream);
	serialize(myNumberOfPlayersToWaitFor, aStream);
}

void CNetworkMessage_ServerReady::DoDeserialize(StreamType& aStream)
{
	CImportantNetworkMessage::DoDeserialize(aStream);
	myNumberOfPlayersToWaitFor = deserialize<unsigned>(aStream);
}

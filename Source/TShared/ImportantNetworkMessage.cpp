#include "stdafx.h"
#include "ImportantNetworkMessage.h"


CImportantNetworkMessage::CImportantNetworkMessage()
{
}


CImportantNetworkMessage::~CImportantNetworkMessage()
{
}

bool CImportantNetworkMessage::IsImportant()
{
	return true;
}

void CImportantNetworkMessage::SetImportantId(int aImportantCount)
{
	myImportantId = aImportantCount;
}

int CImportantNetworkMessage::GetImportantId() const
{
	return myImportantId;
}

void CImportantNetworkMessage::DoSerialize(StreamType& aStream)
{
	CNetworkMessage::DoSerialize(aStream);
	serialize(myImportantId, aStream);
}

void CImportantNetworkMessage::DoDeserialize(StreamType& aStream)
{
	CNetworkMessage::DoDeserialize(aStream);
	myImportantId = deserialize<int>(aStream);
}

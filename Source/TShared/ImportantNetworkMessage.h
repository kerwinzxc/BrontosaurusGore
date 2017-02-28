#pragma once
#include "NetworkMessage.h"


class CImportantNetworkMessage: public CNetworkMessage
{
public:
	CImportantNetworkMessage();
	~CImportantNetworkMessage();

	bool IsImportant() override;

	void SetImportantId(int aImportantCount);
	int GetImportantId()const;

protected:
	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;

private:
	int myImportantId;
};


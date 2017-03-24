#pragma once
#include "ImportantNetworkMessage.h"
#include "../CommonUtilities/vector3.h"
class CNetworkMessage_SetCheckpointMessage : public CImportantNetworkMessage
{
public:
	CNetworkMessage_SetCheckpointMessage();
	~CNetworkMessage_SetCheckpointMessage();

	inline ePackageType GetPackageType()const override;
	inline void SetId(unsigned char aId);
	inline unsigned char GetId() const;

private:
	virtual void DoSerialize(StreamType& aStream);
	virtual void DoDeserialize(StreamType& aStream);

private:
	unsigned char myId;
};

inline ePackageType CNetworkMessage_SetCheckpointMessage::GetPackageType() const
{
	return ePackageType::eSetCheckpointMessage;
}

inline void CNetworkMessage_SetCheckpointMessage::SetId(unsigned char aId)
{
	myId = aId;
}
inline unsigned char CNetworkMessage_SetCheckpointMessage::GetId() const
{
	return myId;
}
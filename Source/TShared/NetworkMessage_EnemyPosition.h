#pragma once
#include "NetworkMessage.h"
#include "../CommonUtilities/vector3.h"

class CNetworkMessage_EnemyPosition : public CNetworkMessage
{
public:
	CNetworkMessage_EnemyPosition();
	~CNetworkMessage_EnemyPosition();
	void SetId(unsigned int anId);
	unsigned int GetId() const;
	void SetPosition(const CU::Vector3f& anEnemyPosition);
	const CU::Vector3f& GetPosition();
	ePackageType GetPackageType() const override;
protected:
	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
private:
	unsigned int myEnemyId;
	CU::Vector3f myEnemyPosition;
};


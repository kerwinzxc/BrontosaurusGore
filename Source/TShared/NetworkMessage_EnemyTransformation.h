#pragma once
#include "NetworkMessage.h"
#include "../CommonUtilities/matrix44.h"

class CNetworkMessage_EnemyTransformation : public CNetworkMessage
{
public:
	CNetworkMessage_EnemyTransformation();
	~CNetworkMessage_EnemyTransformation();
	void SetId(unsigned int aId);
	unsigned int GetId() const;
	void SetTransformation(const CU::Matrix44f& anEnemyTransformaion);
	const CU::Matrix44f& GetTransformation();
	ePackageType GetPackageType() const override;
protected:
	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
private:
	unsigned int myEnemyId;
	CU::Matrix44f myEnemyTransformation;
};


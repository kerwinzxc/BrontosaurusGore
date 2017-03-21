#pragma once
#include "ImportantNetworkMessage.h"
#include "../CommonUtilities/vector3.h"

class CNetworkMessage_OKShoot : public CImportantNetworkMessage
{
public:
	CNetworkMessage_OKShoot();
	~CNetworkMessage_OKShoot();

	ePackageType GetPackageType()const override;

	void SetDirection(const CU::Vector3f& aDirection);
	CU::Vector3f GetDirection();
	void SetWeaponIndex(const unsigned char aWeaponIndex);
	int GetWeaponIndex();

private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;

	CU::Vector3f myDirection;
	unsigned char myWeaponIndex;
};


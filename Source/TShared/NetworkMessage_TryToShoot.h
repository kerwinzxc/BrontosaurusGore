#pragma once
#include "ImportantNetworkMessage.h"
#include "../CommonUtilities/vector3.h"

class CNetworkMessage_TryToShoot : public CImportantNetworkMessage
{
public:
	CNetworkMessage_TryToShoot();
	~CNetworkMessage_TryToShoot();

	ePackageType GetPackageType()const override;

	void SetDirection(const CU::Vector3f& aDirection);
	CU::Vector3f GetDirection();
	void SetWeaponIndex(const unsigned char aWeaponIndex);
	int GetWeaponIndex();
	void SetWeaponName(const std::string aWeaponName);
	const std::string& GetWeaponName();


private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;

	CU::Vector3f myDirection;
	unsigned char myWeaponIndex;
	std::string myWeaponName;
};


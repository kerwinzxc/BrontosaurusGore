#pragma once
#include "ImportantNetworkMessage.h"
#include "../CommonUtilities/vector3.h"

class CNetworkMessage_WeaponShoot : public CImportantNetworkMessage
{
public:
	enum class Shooter : char
	{
		Player,
		Enemy
	};

	CNetworkMessage_WeaponShoot();
	~CNetworkMessage_WeaponShoot();

	ePackageType GetPackageType()const override;

	void SetDirection(const CU::Vector3f& aDirection);
	CU::Vector3f GetDirection();
	void SetWeaponIndex(const unsigned char aWeaponIndex);
	int GetWeaponIndex();
	Shooter GetShooter();
	void SetShooterId(unsigned int anId);
	unsigned GetId();
	void SetShooter(Shooter aShooter);
private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;

	CU::Vector3f myDirection;
	unsigned char myWeaponIndex;
	Shooter myShooter;
	unsigned myId;
};


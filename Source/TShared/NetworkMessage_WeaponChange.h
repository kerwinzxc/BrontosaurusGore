#pragma once
#include "ImportantNetworkMessage.h"
#include "../Components/ComponentMessage.h"

class CNetworkMessage_WeaponChange : public CImportantNetworkMessage
{


public:
	enum class Shooter : char
	{
		Player,
		Enemy
	};


	CNetworkMessage_WeaponChange();
	~CNetworkMessage_WeaponChange();
	void SetWeapon(unsigned short aActiveWeaponIndex);
	ePackageType GetPackageType() const override;
	int GetWeaponIndex() const;
	Shooter GetShooter();
	void SetShooterId(unsigned int anId);
	unsigned GetId();
	void SetShooter(Shooter aShooter);
protected:
	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
	unsigned short myWeaponIndex;
	Shooter myShooter;
	unsigned myId;
};


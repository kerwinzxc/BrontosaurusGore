#pragma once
#include "ImportantNetworkMessage.h"
#include "../Components/ComponentMessage.h"

class CNetworkMessage_WeaponChange : public CImportantNetworkMessage
{
public:
	CNetworkMessage_WeaponChange();
	~CNetworkMessage_WeaponChange();
	void SetWeapon(unsigned short aActiveWeaponIndex);
	ePackageType GetPackageType() const override;
	int GetWeaponIndex() const;
protected:
	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;
	unsigned short myWeaponIndex;
};


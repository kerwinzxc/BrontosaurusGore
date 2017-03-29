#pragma once
#include "ImportantNetworkMessage.h"
#include "../Components/EnemyTypes.h"
class CNetworkMessage_SpawnEnemyRepesention: public CImportantNetworkMessage
{
public:
	CNetworkMessage_SpawnEnemyRepesention();
	~CNetworkMessage_SpawnEnemyRepesention();

	ePackageType GetPackageType()const override;

	void SetHealth(const short aHealthValue);
	void SetEnemyType(const eEnemyTypes aType);

	const short GetHealth() const;
	const eEnemyTypes GetEnemyType() const;

private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;

	short myEnemyLife;
	eEnemyTypes myEnemyType;

};


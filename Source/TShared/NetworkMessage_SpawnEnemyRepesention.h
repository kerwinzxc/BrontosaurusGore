#pragma once
#include "ImportantNetworkMessage.h"
#include "../Components/EnemyTypes.h"
#include "../CommonUtilities/vector3.h"
class CNetworkMessage_SpawnEnemyRepesention: public CImportantNetworkMessage
{
public:
	CNetworkMessage_SpawnEnemyRepesention();
	~CNetworkMessage_SpawnEnemyRepesention();

	ePackageType GetPackageType()const override;

	void SetHealth(const short aHealthValue);
	void SetEnemyType(const eEnemyTypes aType);
	void SetPosition(const CU::Vector3f& aPosition);

	const short GetHealth() const;
	const eEnemyTypes GetEnemyType() const;
	const CU::Vector3f GetPosition() const;

private:
	void DoSerialize(StreamType& aStream) override;
	void DoDeserialize(StreamType& aStream) override;

	short myEnemyLife;
	eEnemyTypes myEnemyType;
	CU::Vector3f myPosition;

};


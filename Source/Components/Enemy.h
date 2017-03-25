#pragma once
#include "Component.h"
#include "EnemyBlueprint.h"

class CGameObject;

class CEnemy : public CComponent
{
public:
	CEnemy(unsigned int aId);
	~CEnemy();

	virtual inline void SetEnemyData(const SEnemyBlueprint& aData);
	static void SetPlayerObject(CGameObject* aPlayerObj);

	virtual void Attack();
	void Update(const float aDeltaTime);
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	virtual	void ChangeWeapon(const unsigned int aIndex);
protected:
	virtual	CU::Vector3f ClosestPlayerPosition();
	virtual void UpdateTransformation();
	virtual	void MoveForward(const float aMovAmount);

	virtual inline bool WithinDetectionRange(const float aDist);
	virtual inline bool WithinAttackRange(const float aDist);
	virtual inline bool OutsideAttackRange(const float aDist);

protected:
	static CU::GrowingArray<CGameObject*> ourPlayerObjects;
	unsigned int myServerId;
	unsigned int myActiveWeaponIndex;

	float mySpeed;
	float myDetectionRange2;
	float myStartAttackRange2;
	float myStopAttackRange2;
	float myNetworkPositionUpdateCoolDown;
	float myElapsedWaitingToSendMessageTime;

	bool myIsDead;
	bool myIsAttacking;
};

inline void CEnemy::SetEnemyData(const SEnemyBlueprint& aData)
{
	mySpeed = aData.speed;
	myDetectionRange2 = aData.detectionRange * aData.detectionRange;
	myStartAttackRange2 = aData.startAttackRange * aData.startAttackRange;
	myStopAttackRange2 = aData.stopAttackRange * aData.stopAttackRange;
}

inline bool  CEnemy::WithinDetectionRange(const float aDist)
{
	return aDist < myDetectionRange2;
}

inline bool  CEnemy::WithinAttackRange(const float aDist)
{
	return aDist < myStartAttackRange2;
}

inline bool  CEnemy::OutsideAttackRange(const float aDist)
{
	return aDist > myStopAttackRange2;
}
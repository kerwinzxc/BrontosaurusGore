#pragma once
#include "Component.h"
#include "EnemyBlueprint.h"
#include "EnemyTypes.h"
#include "../ThreadedPostmaster/Subscriber.h"

class CGameObject;

class CEnemy : public CComponent, public Postmaster::ISubscriber
{
public:
	CEnemy(unsigned int aId, eEnemyTypes aType);
	~CEnemy();

	void UpdateBaseMemberVars(const float aDeltaTime);
	virtual inline void SetEnemyData(const SEnemyBlueprint* aData);
	static void SetPlayerObject(CGameObject* aPlayerObj);

	virtual void Attack();
	virtual void Update(const float aDeltaTime) = 0;
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	virtual	void ChangeWeapon(const unsigned int aIndex);
	inline eEnemyTypes GetEnemyType();
	inline void SetType(const eEnemyTypes aType);

	eMessageReturn DoEvent(const CResetToCheckPointMessage& aResetToCheckPointMessage) override;
	void Init();
protected:
	void CheckForNewTransformation(const float aDeltaTime);
	void SendTransformationToServer();
	virtual CU::Vector3f ClosestPlayerPosition();
	virtual	void MoveForward(const float aMovAmount);

	virtual inline bool WithinDetectionRange();
	virtual inline bool WithinAttackRange();
	virtual inline bool OutsideAttackRange();
	virtual inline bool WithinWalkToMeleeRange();
	void LookAtPlayer();
protected:
	static CU::GrowingArray<CGameObject*> ourPlayerObjects;

	CU::Vector3f myVelocity;
	CU::Vector3f myClosestPlayerPos;
	CU::Vector3f myPos;
	CU::Vector3f myToPlayer;
	CU::Vector3f mySpawnPosition;

	float myDistToPlayer;

	unsigned int myServerId;
	unsigned int myActiveWeaponIndex;

	float mySpeed;
	float myDetectionRange2;
	float myStartAttackRange2;
	float myStopAttackRange2;
	float myWalkToMeleeRange2;
	float myNetworkPositionUpdateCoolDown;
	float myElapsedWaitingToSendMessageTime;

	eEnemyTypes myType;
	bool myIsDead;
	bool myIsAttacking;
	char myControllerConstraints;
};

inline void CEnemy::SetEnemyData(const SEnemyBlueprint* aData)
{
	mySpeed = aData->speed;
	myDetectionRange2 = aData->detectionRange * aData->detectionRange;
	myStartAttackRange2 = aData->startAttackRange * aData->startAttackRange;
	myStopAttackRange2 = aData->stopAttackRange * aData->stopAttackRange;
	myWalkToMeleeRange2 = aData->walkToMeleeRange *  aData->walkToMeleeRange;
}

inline bool  CEnemy::WithinDetectionRange()
{
	return myDistToPlayer < myDetectionRange2;
}

inline bool CEnemy::WithinAttackRange()
{
	return myDistToPlayer < myStartAttackRange2;
}

inline bool CEnemy::WithinWalkToMeleeRange()
{
	return myDistToPlayer < myWalkToMeleeRange2;
}

inline bool CEnemy::OutsideAttackRange()
{
	return myDistToPlayer > myStopAttackRange2;
}

inline eEnemyTypes CEnemy::GetEnemyType()
{
	return myType;
}

inline void CEnemy::SetType(const eEnemyTypes aType)
{
	myType = aType;
}

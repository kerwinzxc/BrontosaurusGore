#pragma once
#include "Component.h"
#include "EnemyBlueprint.h"
#include "EnemyTypes.h"
#include "../ThreadedPostmaster/Subscriber.h"
#include "../CommonUtilities/vector4.h"
#include "..\Audio\AudioInterface.h"

class CGameObject;
class CEnemyRunTowardsComponent;

class CEnemy : public CComponent, public Postmaster::ISubscriber
{
public:
	CEnemy(unsigned int aId, eEnemyTypes aType);
	~CEnemy();

	
	void UpdateBaseMemberVars(const float aDeltaTime);
	virtual inline void SetEnemyData(const SEnemyBlueprint* aData);
	static void SetPlayerObject(CGameObject* aPlayerObj);
	static void AddEnemyRunTowardsComponent(CEnemyRunTowardsComponent* aEnemyRunTowardsComponent);
	static void playerDontFuckTHisUp();
	virtual void Attack();
	virtual void Update(const float aDeltaTime) = 0;
	void StartHighlight();
	void DoSplatter();
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;
	void KillEverythingThenResetItAgain(const bool aFlag);
	virtual	void ChangeWeapon(const unsigned int aIndex);
	inline eEnemyTypes GetEnemyType();
	inline void SetType(const eEnemyTypes aType);
	virtual inline void Sthap();

	eMessageReturn DoEvent(const CResetToCheckPointMessage& aResetToCheckPointMessage) override;
	virtual void Init();
	inline const bool GetIsDead() const; 
	inline const unsigned short GetNetworkID() const;
	
	void DoDamageHighlight(const float aDeltaTime);

protected:
	void ChangeClientAnimation(const eComponentMessageType aMessageType) const;
	void FreeJumpPoint();
	void SetHighlight(const CU::Vector4f& aColor, float aIntensity);
	void CheckForNewTransformation(const float aDeltaTime);
	void SendTransformationToServer();
	virtual CU::Vector3f ClosestPlayerPosition();
	virtual	void MoveForward(const float aMovAmount);

	virtual inline bool WithinDetectionRange();
	virtual inline bool WithinShootRange();
	virtual inline bool WithinAttackRange();
	virtual inline bool OutsideAttackRange();
	virtual inline bool WithinWalkToMeleeRange();
	virtual void DoDeathEffect() {} // only in clientRepresentaion.
	void LookAtPlayer();
	bool GetIfSidesAreColliding();
	const CU::Vector3f GetNearestJumpPosition();
protected:
	static CU::GrowingArray<CGameObject*> ourPlayerObjects;
	static CU::GrowingArray<CEnemyRunTowardsComponent*> ourEnemyRunTowardsComponents;

	CU::Vector3f myVelocity;
	CU::Vector3f myClosestPlayerPos;
	CU::Vector3f myPos;
	CU::Vector3f myToPlayer;
	CU::Vector3f mySpawnPosition;

	Audio::GameObjectID myAudioID;
	float myDistToPlayer;

	unsigned int myServerId;
	unsigned int myActiveWeaponIndex;
	unsigned int myRunTowardsComponentIndex;

	float mySpeed;
	float myDetectionRange2;
	float myWeaponRange2;
	float myStartAttackRange2;
	float myStopAttackRange2;
	float myWalkToMeleeRange2;
	float myNetworkPositionUpdateCoolDown;
	float myElapsedWaitingToSendMessageTime;

	eEnemyTypes myType;
	bool myIsDead;
	bool myIsAttacking;
	bool myShouldNotReset;
	bool myIsAggressive;
	char myControllerConstraints;
	float myBloodSplatterTimer;
private:

	float myHighlightTimer;
	bool myDoingHighlight;
};

inline void CEnemy::SetEnemyData(const SEnemyBlueprint* aData)
{
	mySpeed = aData->speed;
	myDetectionRange2 = aData->detectionRange * aData->detectionRange;
	myStartAttackRange2 = aData->startAttackRange * aData->startAttackRange;
	myStopAttackRange2 = aData->stopAttackRange * aData->stopAttackRange;
	myWalkToMeleeRange2 = aData->walkToMeleeRange *  aData->walkToMeleeRange;
	myWeaponRange2 = aData->shootingRange * aData->shootingRange;
}

inline bool  CEnemy::WithinDetectionRange()
{
	return myDistToPlayer < myDetectionRange2;
}

inline bool CEnemy::WithinShootRange()
{
	return myDistToPlayer < myWeaponRange2;
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

inline const bool CEnemy::GetIsDead() const
{
	return myIsDead;
}

inline const unsigned short CEnemy::GetNetworkID() const
{
	return myServerId;
}

inline void CEnemy::Sthap()
{

}
#pragma once
#include "Component.h"
#include "EnemyBlueprint.h"

class CGameObject;

namespace Component
{
	class CEnemy : public CComponent
	{
	public:
		CEnemy(unsigned int aId);
		~CEnemy();

		inline void SetEnemyData(const SEnemyBlueprint& aData);
		static void SetPlayerObject(CGameObject* aPlayerObj);

		void Attack();
		void Update(const CU::Time& aDeltaTime);
		void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	private:
		CU::Vector3f ClosestPlayerPosition();
		void UpdateTransformation();
		void MoveForward(const float aMovAmount);

		inline bool WithinDetectionRange(const float aDist);
		inline bool WithinAttackRange(const float aDist);
		inline bool OutsideAttackRange(const float aDist);

	protected:
		static CU::GrowingArray<CGameObject*> ourPlayerObjects;
		unsigned int myServerId;

		float mySpeed;
		float myDetectionRange2;
		float myStartAttackRange2;
		float myStopAttackRange2;

		bool myIsDead;
		bool myIsAttacking;
	};
}

inline void Component::CEnemy::SetEnemyData(const SEnemyBlueprint& aData)
{
	mySpeed = aData.speed;
	myDetectionRange2 = aData.detectionRange * aData.detectionRange;
	myStartAttackRange2 = aData.startAttackRange * aData.startAttackRange;
	myStopAttackRange2 = aData.stopAttackRange * aData.stopAttackRange;
}

inline bool  Component::CEnemy::WithinDetectionRange(const float aDist)
{
	return aDist < myDetectionRange2;
}

inline bool  Component::CEnemy::WithinAttackRange(const float aDist)
{
	return aDist < myStartAttackRange2;
}

inline bool  Component::CEnemy::OutsideAttackRange(const float aDist)
{
	return aDist > myStopAttackRange2;
}
#pragma once
#include "Component.h"

namespace Component
{
	class CEnemy : public CComponent
	{
	public:
		CEnemy(unsigned int anId);
		~CEnemy();
		void SetHealth(const unsigned aHealth);
		void SetSpeed(const float aSpeed);
		void SetDetectionRange(const float aDetactionRange);
		void SetStartAttackRange(const float aStartAttackRange);
		void SetStopAttackRange(const float aStopAttackRange);

		void Move(const CU::Vector3f& aDispl);
		
		void Update(const CU::Time& aDeltaTime);

		void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

		static void SetPlayer(CGameObject* playerObject);
	protected:
		void Attack();
		CU::Vector3f ClosestPlayerPosition();
		static CU::GrowingArray<CGameObject*> myPlayerObject;
		unsigned myHealth;
		float mySpeed;
		float myDetectionRange2;
		float myStartAttackRange2;
		float myStopAttackRange2;
		bool myIsDead;
		unsigned myServerId;
		bool myAttacking;
	};
}

inline void Component::CEnemy::SetHealth(const unsigned aHealth)
{
	myHealth = aHealth;
}

inline void Component::CEnemy::SetSpeed(const float aSpeed)
{
	mySpeed = aSpeed *100 ;
}

inline void Component::CEnemy::SetDetectionRange(const float aDetactionRange)
{
	myDetectionRange2 = aDetactionRange * aDetactionRange;
}

inline void Component::CEnemy::SetStartAttackRange(const float aStartAttackRange)
{
	myStartAttackRange2 = aStartAttackRange  * aStartAttackRange;
}

inline void Component::CEnemy::SetStopAttackRange(const float aStopAttackRange)
{
	myStopAttackRange2 = aStopAttackRange * aStopAttackRange;
}
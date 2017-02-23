#pragma once
#include "Component.h"

namespace Component
{
	class CEnemy : public CComponent
	{
	public:
		CEnemy();
		~CEnemy();
		void SetHealth(const unsigned aHealth);
		void SetSpeed(const float aSpeed);
		void SetDetectionRange(const float aDetactionRange);
		void SetStartAttackRange(const float aStartAttackRange);
		void SetStopAttackRange(const float aStopAttackRange);

		void Update(const CU::Time& aDeltaTime);

		static void SetPlayer(CGameObject* playerObject);
	protected:
		static CGameObject* myPlayerObject;
		unsigned myHealth;
		float mySpeed;
		float myDetectionRange2;
		float myStartAttackRange2;
		float myStopAttackRange2;
	};
}

inline void Component::CEnemy::SetHealth(const unsigned aHealth)
{
	myHealth = aHealth;
}

inline void Component::CEnemy::SetSpeed(const float aSpeed)
{
	mySpeed = aSpeed;
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
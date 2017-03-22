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

		void Update(const CU::Time& aDeltaTime);
		void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

	private:
		CU::Vector3f ClosestPlayerPosition();
		void UpdateTransformation();
		void MoveForward(const float aMovAmount);

	protected:
		static CU::GrowingArray<CGameObject*> ourPlayerObjects;

		unsigned myHealth;
		float mySpeed;
		float myDetectionRange2;
		float myStartAttackRange2;
		float myStopAttackRange2;

		bool myIsDead;
		unsigned myServerId;
	};
}

inline void Component::CEnemy::SetEnemyData(const SEnemyBlueprint& aData)
{
	myHealth = aData.health;
	mySpeed = aData.health;
	myDetectionRange2 = aData.detectionRange * aData.detectionRange;
	myStartAttackRange2 = aData.startAttackRange * aData.startAttackRange;
	myStopAttackRange2 = aData.stopAttackRange * aData.stopAttackRange;
}
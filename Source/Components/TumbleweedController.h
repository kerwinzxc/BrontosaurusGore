#pragma once
#include "Component.h"

namespace Physics
{
	class CPhysicsScene;
}

class CTumbleweedController : public CComponent
{
public:
	CTumbleweedController();
	~CTumbleweedController();
	void Init(Physics::CPhysicsScene* aPhysicsScene);
	void Update(const float aDeltaTime);
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void RandomizeDirection();
	void SetDirection(const CU::Vector3f& aDirection);
	void RandomizeTime();
private:
	Physics::CPhysicsScene* myPhysicsScene;
	CU::Vector3f myDirection;
	float myAddForceCountdown;
	float myForce;
	void RandomizeForce();
};


#pragma once
#include "../CommonUtilities/vector3.h"
#include "../CommonUtilities/vector4.h"

class CComponent;
class CRigidBodyComponent;

class CRigidBody
{
public:
	friend class CPhysicsManager;
	CRigidBody();
	CRigidBody(float aMass);
	~CRigidBody();

	void Update();

	CU::Vector3f GetVeolocity();
	void AddImpulse(const CU::Vector3f& anImpulse);

private:
	void SetComponent(CRigidBodyComponent* aComponent);

private:

	float myMass;
	CU::Vector3f myVelocity;
	CU::Vector3f myPrevoiusPosition;
	CU::Vector3f myPrevoiusImpulse;
	CComponent* myComponent;
};

inline CU::Vector3f CRigidBody::GetVeolocity()
{
	return myVelocity;
}

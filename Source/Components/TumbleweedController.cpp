#include "stdafx.h"
#include "TumbleweedController.h"
#include "..\Physics\PhysicsScene.h"

CTumbleweedController::CTumbleweedController()
{

	RandomizeTime();
	RandomizeDirection();
	RandomizeForce();
	myPhysicsScene = nullptr;

}

CTumbleweedController::~CTumbleweedController()
{
	myPhysicsScene = nullptr;
}

void CTumbleweedController::Init(Physics::CPhysicsScene* aPhysicsScene)
{
	myPhysicsScene = aPhysicsScene;
}

void CTumbleweedController::Update(const float aDeltaTime)
{
	myAddForceCountdown -= aDeltaTime;
	if(myAddForceCountdown < 0.0f)
	{
		SComponentMessageData addForceData;
		addForceData.myVector3f = myDirection * myForce;
		GetParent()->NotifyComponents(eComponentMessageType::eAddForce, addForceData);
	
		RandomizeForce();
		RandomizeDirection();
		RandomizeTime();
	}
}

void CTumbleweedController::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eTakeDamage:
	{
		SComponentMessageData addForceData;
		myDirection.x = aMessageData.myVector4f.y;
		myDirection.y = aMessageData.myVector4f.z;
		myDirection.z = aMessageData.myVector4f.w;
		myDirection.Normalize();

		addForceData.myVector3f = myDirection * aMessageData.myInt * 100.f ;

		//addForceData.myVector3f *= (float)aMessageData.myInt;
		GetParent()->NotifyComponents(eComponentMessageType::eAddForce, addForceData);
		break;
	}
	default:
		break;
	}
	aMessageData;
	aMessageType;
}

void CTumbleweedController::RandomizeDirection()
{
	float randomX = RAND_FLOAT * 2.0f;
	float randomZ = RAND_FLOAT * 2.0f;
	randomX -= 1.0f;
	randomZ -= 1.0f;
	myDirection.x = randomX;
	myDirection.y = 0.1f;
	myDirection.z = randomZ;
	myDirection.Normalize();
}
void CTumbleweedController::SetDirection(const CU::Vector3f& aDirection)
{
	myDirection = aDirection;
}

void CTumbleweedController::RandomizeTime()
{
	myAddForceCountdown = static_cast<float>(rand() % 15) + 5.0f;

}

void CTumbleweedController::RandomizeForce()
{
	myForce = fmod(rand(), 3000.f);
	myForce += 2000.f;
}

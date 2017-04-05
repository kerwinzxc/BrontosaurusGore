#include "stdafx.h"
#include "EnemyClientRepresentation.h"


CEnemyClientRepresentation::CEnemyClientRepresentation(unsigned int anId, const eEnemyTypes aType)
	:CEnemy(anId, aType)
{
	myComponentId = anId;
	myPositionInterpolationSpeed = 6.1f;
	myRotationInterpolationSpeed = 7.1f;
	myType = aType;
}


CEnemyClientRepresentation::~CEnemyClientRepresentation()
{
}

void CEnemyClientRepresentation::SetFutureMatrix(const CU::Matrix44f& aMatrix)
{
	myFutureMatrix = aMatrix;
}

void CEnemyClientRepresentation::Update(float aDeltaTime)
{
	GetParent()->GetLocalTransform().Lerp(myFutureMatrix, myRotationInterpolationSpeed * aDeltaTime);
	GetParent()->GetLocalTransform().SetPosition(GetParent()->GetLocalTransform().GetPosition().Lerp(myFutureMatrix.GetPosition(), myPositionInterpolationSpeed * aDeltaTime));
	GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
	SComponentMessageData positiondata;
	positiondata.myVector3f = GetParent()->GetLocalTransform().GetPosition();
	GetParent()->NotifyComponents(eComponentMessageType::eSetControllerPosition, positiondata);

	DoDamageHighlight(aDeltaTime);
}

void  CEnemyClientRepresentation::Init()
{
	myFutureMatrix = GetParent()->GetToWorldTransform();
}

void CEnemyClientRepresentation::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	CEnemy::Receive(aMessageType, aMessageData);
	switch (aMessageType)
	{
	case eComponentMessageType::eTakeDamage:
	case eComponentMessageType::eTookDamage:
		StartHighlight();
		break;
	}
}

#include "stdafx.h"
#include "EnemyClientRepresentation.h"


CEnemyClientRepresentation::CEnemyClientRepresentation(unsigned int anId) : myComponentId(anId)
{
	myPositionInterpolationSpeed = 3.1;
	myRotationInterpolationSpeed = 7.1f;
	myType = eEnemyTypes::eDefault;
}


CEnemyClientRepresentation::~CEnemyClientRepresentation()
{
}

void CEnemyClientRepresentation::SetFutureMatrix(const CU::Matrix44f& aMatrix)
{
	myFutureMatrix = aMatrix;
}

void  CEnemyClientRepresentation::Update(float aDeltaTime)
{
	GetParent()->GetLocalTransform().Lerp(myFutureMatrix, myRotationInterpolationSpeed * aDeltaTime);
	GetParent()->GetLocalTransform().SetPosition(GetParent()->GetLocalTransform().GetPosition().Lerp(myFutureMatrix.GetPosition(), myPositionInterpolationSpeed * aDeltaTime));
	GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
}

void  CEnemyClientRepresentation::Init()
{
	myFutureMatrix = GetParent()->GetToWorldTransform();
}
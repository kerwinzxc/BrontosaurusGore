#include "stdafx.h"
#include "EnemyClientRepresentation.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/AddToCheckPointResetList.h"

CEnemyClientRepresentation::CEnemyClientRepresentation(unsigned int anId, const eEnemyTypes aType)
	:CEnemy(anId, aType)
{
	myComponentId = anId;
	myPositionInterpolationSpeed = 6.1f;
	myRotationInterpolationSpeed = 7.1f;
	myType = aType;
	myIsAlive = true;
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
	if(myIsAlive == true)
	{
		GetParent()->GetLocalTransform().Lerp(myFutureMatrix, myRotationInterpolationSpeed * aDeltaTime);
		GetParent()->GetLocalTransform().SetPosition(GetParent()->GetLocalTransform().GetPosition().Lerp(myFutureMatrix.GetPosition(), myPositionInterpolationSpeed * aDeltaTime));
		GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
		SComponentMessageData positiondata;
		positiondata.myVector3f = GetParent()->GetLocalTransform().GetPosition();
		GetParent()->NotifyComponents(eComponentMessageType::eSetControllerPosition, positiondata);

		DoDamageHighlight(aDeltaTime);
		CheckIfOutOfBounds();
	}
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
	case eComponentMessageType::eDied:
	{
		CU::Vector3f hellPosition(-9999.0f, -99999.0f, -99999.0f);
		SComponentMessageData controllerPositionData;
		controllerPositionData.myVector3f = hellPosition;
		GetParent()->NotifyOnlyComponents(eComponentMessageType::eSetControllerPosition, controllerPositionData);
		CAddToCheckPointResetList* addToCheckPointMessage = new CAddToCheckPointResetList(GetParent());
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(addToCheckPointMessage);
		myIsAlive = false;
		break;
	}
	case eComponentMessageType::eCheckPointReset:
	{
		SComponentMessageData controllerPositionData;
		controllerPositionData.myVector3f = GetParent()->GetWorldPosition();
		GetParent()->NotifyOnlyComponents(eComponentMessageType::eSetControllerPosition, controllerPositionData);
		myIsAlive = true;
	}
	}
}

void CEnemyClientRepresentation::CheckIfOutOfBounds()
{
	if (GetParent()->GetWorldPosition().y < -100.0f)
	{
		SComponentQuestionData healthQuestionData;
		if(GetParent()->AskComponents(eComponentQuestionType::eGetHealth, healthQuestionData) == true)
		{
			if(healthQuestionData.myInt > 0)
			{
				SComponentMessageData takeDamageData;
				takeDamageData.myInt = 10000;
				GetParent()->NotifyComponents(eComponentMessageType::eTakeDamage, takeDamageData);			
			}
		}
		//Teleport stuff back code
		//CU::Vector3f teleportPosition(parentTransform.GetPosition().x, parentTransform.GetPosition().y * -1, parentTransform.GetPosition().z);
		////parentTransform.SetPosition(parentTransform.GetPosition().x, parentTransform.GetPosition().y * -1, parentTransform.GetPosition().z);
		//SComponentQuestionData data;
		//data.myVector4f = (teleportPosition - parentTransform.GetPosition()) ;
		//data.myVector4f.w = aDeltaTime.GetSeconds();
		//if (GetParent()->AskComponents(eComponentQuestionType::eMovePhysicsController, data) == true)
		//{
		//	parentTransform.SetPosition(data.myVector3f);
		//}
	}
}

bool CEnemyClientRepresentation::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	switch (aQuestionType)
	{
	case eComponentQuestionType::eIsEnemy :
	{
		return true;
		break;
	}
	default:
		break;
	}
}
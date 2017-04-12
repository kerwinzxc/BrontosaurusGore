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
		if (myAudioID != 0)
		{
			const CU::Matrix44f transform = GetParent()->GetToWorldTransform();
			Audio::CAudioInterface::GetInstance()->SetGameObjectPosition(myAudioID, transform.GetPosition(), {0.f,0.f,0.f});
		}

		GetParent()->GetLocalTransform().Lerp(myFutureMatrix, myRotationInterpolationSpeed * aDeltaTime);
		GetParent()->GetLocalTransform().SetPosition(GetParent()->GetLocalTransform().GetPosition().Lerp(myFutureMatrix.GetPosition(), myPositionInterpolationSpeed * aDeltaTime));
		GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
		SComponentMessageData positiondata;
		positiondata.myVector3f = GetParent()->GetLocalTransform().GetPosition();
		GetParent()->NotifyComponents(eComponentMessageType::eSetControllerPosition, positiondata);

		if(myBloodSplatterTimer > 0.f)
		{
			myBloodSplatterTimer -= aDeltaTime;
		}

		DoDamageHighlight(aDeltaTime);
		CheckIfOutOfBounds();
	}

	if (myIsAlive == false)
	{
		CU::Vector3f hellPosition(-9999.0f, -99999.0f, -99999.0f);
		SComponentMessageData controllerPositionData;
		controllerPositionData.myVector3f = hellPosition;
		GetParent()->NotifyOnlyComponents(eComponentMessageType::eActivate, SComponentMessageData());
		GetParent()->NotifyOnlyComponents(eComponentMessageType::eSetControllerPosition, controllerPositionData);
		GetParent()->SetWorldPosition(hellPosition);
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
		DL_PRINT("Wants to take damage");
		if(myIsAlive == false)
		{
			DL_PRINT("dead enemy wants to take damage");
		}
		DoSplatter();
		StartHighlight();
		break;
	case eComponentMessageType::eDoStuffBeforeDie:
	{
		DoDeathEffect();
		CU::Vector3f hellPosition(-9999.0f, -99999.0f, -99999.0f);
		SComponentMessageData controllerPositionData;
		controllerPositionData.myVector3f = hellPosition;
		GetParent()->NotifyOnlyComponents(eComponentMessageType::eSetControllerPosition, controllerPositionData);
		GetParent()->SetWorldPosition(hellPosition);
		myFutureMatrix.SetPosition(hellPosition);
		GetParent()->NotifyOnlyComponents(eComponentMessageType::eMoving, SComponentMessageData());
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
		GetParent()->NotifyOnlyComponents(eComponentMessageType::eActivate, SComponentMessageData());
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

void CEnemyClientRepresentation::DoDeathEffect()
{
	switch (myType)
	{
	case eEnemyTypes::eImp:
		Audio::CAudioInterface::GetInstance()->PostEvent("Imp_Death", myAudioID);
		break;
	case eEnemyTypes::eRevenant:
		Audio::CAudioInterface::GetInstance()->PostEvent("Revenant_Death", myAudioID);
		break;
	case eEnemyTypes::ePinky:
		Audio::CAudioInterface::GetInstance()->PostEvent("Pinky_Death", myAudioID);
		break;
	default:
		break;
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
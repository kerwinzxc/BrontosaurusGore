#include "stdafx.h"
#include "EnemyClientRepresentation.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/AddToCheckPointResetList.h"
#include "PollingStation.h"

CEnemyClientRepresentation::CEnemyClientRepresentation(unsigned int anId, const eEnemyTypes aType)
	:CEnemy(anId, aType)
{
	myComponentId = anId;
	myPositionInterpolationSpeed = 6.1f;
	myRotationInterpolationSpeed = 7.1f;
	myType = aType;
	myIsAlive = true;
	myShootChargeCountDown = 0.0f;
	myAmountOfCuedShots = 0;
	myInvisibleAfterDeathCountdown = 0.0f;
	myShouldReset = true;
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

		if (myShootChargeCountDown > 0.0f)
		{
			myShootChargeCountDown -= aDeltaTime;
			if (myShootChargeCountDown <= 0.0f)
			{
				SComponentMessageData shotData;
				shotData.myVector3f = myShootDirection;
				GetParent()->NotifyComponents(eComponentMessageType::eShootWithNetworking, shotData);
				if (myAmountOfCuedShots > 0)
				{
					myAmountOfCuedShots--;
					myShootChargeCountDown = 1.1f;
				}
			}
		}
	}
	SComponentMessageData updateData;
	updateData.myFloat = aDeltaTime;
	GetParent()->NotifyComponents(eComponentMessageType::eUpdatePinky, updateData);
	if (myIsAlive == false)
	{
		GetParent()->GetLocalTransform().Lerp(myFutureMatrix, myRotationInterpolationSpeed * aDeltaTime);
		GetParent()->GetLocalTransform().SetPosition(GetParent()->GetLocalTransform().GetPosition().Lerp(myFutureMatrix.GetPosition(), myPositionInterpolationSpeed * aDeltaTime));
		GetParent()->NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());

		CU::Vector3f hellPosition(-9999.0f, -99999.0f, -99999.0f);
		SComponentMessageData controllerPositionData;
		controllerPositionData.myVector3f = hellPosition;
		//GetParent()->NotifyOnlyComponents(eComponentMessageType::eActivate, SComponentMessageData());
		SComponentMessageData coolIcanseedeath;
		coolIcanseedeath.myBool = true;
		GetParent()->NotifyOnlyComponents(eComponentMessageType::eSetVisibility, coolIcanseedeath);
		GetParent()->NotifyOnlyComponents(eComponentMessageType::eSetControllerPosition, controllerPositionData);
		//GetParent()->SetWorldPosition(hellPosition);

		if (myInvisibleAfterDeathCountdown > 0.0f)
		{
			myInvisibleAfterDeathCountdown -= aDeltaTime;
		}
		else
		{
			SComponentMessageData visibilityData;
			visibilityData.myBool = false;
			GetParent()->NotifyComponents(eComponentMessageType::eSetVisibility, visibilityData);
		}
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
		//GetParent()->SetWorldPosition(hellPosition);
		//myFutureMatrix.SetPosition(hellPosition);
		GetParent()->NotifyOnlyComponents(eComponentMessageType::eMoving, SComponentMessageData());
		CAddToCheckPointResetList* addToCheckPointMessage = new CAddToCheckPointResetList(GetParent());
		Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(addToCheckPointMessage);
		myIsAlive = false;
		myInvisibleAfterDeathCountdown = 1.2f;
		break;
	}
	case eComponentMessageType::eCheckPointReset:
	{
		if(myShouldReset == true)
		{
			SComponentMessageData controllerPositionData;
			controllerPositionData.myVector3f = GetParent()->GetWorldPosition();
			GetParent()->NotifyOnlyComponents(eComponentMessageType::eSetControllerPosition, controllerPositionData);
			GetParent()->NotifyOnlyComponents(eComponentMessageType::eActivate, SComponentMessageData());
			myIsAlive = true;	
		}
		else
		{
			CU::Vector3f hellPosition(-9999.0f, -99999.0f, -99999.0f);
			SComponentMessageData controllerPositionData;
			controllerPositionData.myVector3f = hellPosition;
			GetParent()->NotifyOnlyComponents(eComponentMessageType::eSetControllerPosition, controllerPositionData);
			GetParent()->SetWorldPosition(hellPosition);
			myFutureMatrix.SetPosition(hellPosition);
			GetParent()->NotifyOnlyComponents(eComponentMessageType::eMoving, SComponentMessageData());
		}
	}
	case eComponentMessageType::eChargeShootWithNetworking :
	{
		myShootDirection = aMessageData.myVector3f;
		if(myShootChargeCountDown > 0)
		{
			myAmountOfCuedShots++;
		}
		else
		{
			myShootChargeCountDown = 1.1f;
		}
		break;
	}
	}
}

void CEnemyClientRepresentation::CheckIfOutOfBounds()
{
	short levelIndex = CPollingStation::GetInstance()->GetCurrentLevelIndex();
	float zKillValue = -100.0f;
	if(levelIndex = 3)
	{
		zKillValue = -1000.0f;
	}
	if (GetParent()->GetWorldPosition().y < zKillValue)
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

void CEnemyClientRepresentation::DeactivateReset()
{
	myShouldReset = false;
}
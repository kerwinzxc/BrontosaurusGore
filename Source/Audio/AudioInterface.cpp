#include "AudioInterface.h"
#include "WwiseManager.h"
#define SAFE_DELETE(ptr) delete ptr; ptr = nullptr

namespace Audio
{

	CAudioInterface* CAudioInterface::myInstance = nullptr;
	CAudioInterface::CAudioInterface()
		:myWwiseManager(new CWwiseManager()), myGameObjectCounter(0)
	{
	}

	CAudioInterface::~CAudioInterface()
	{
		SAFE_DELETE(myWwiseManager);
	}

	bool CAudioInterface::Init(const char* aInitBank)
	{
		bool result;
		result = myWwiseManager->Init(aInitBank);

		myGameObjectID = RegisterGameObject();
		return result;
	}

	void CAudioInterface::Update()
	{
		if (myWwiseManager)
		{
			myWwiseManager->Update();
		}

	}
	static const float SoundSceneScale = 1.f;
	void CAudioInterface::SetListenerPosition(const CU::Matrix44f & aTranslation)
	{
		AkListenerPosition listenerposition;

		const CU::Vector3f position = aTranslation.myPosition * SoundSceneScale;
		listenerposition.Position.X = position.x;
		listenerposition.Position.Y = position.y;
		listenerposition.Position.Z = position.z;
		
		listenerposition.OrientationFront.X = aTranslation.myForwardVector.x;
		listenerposition.OrientationFront.Y = aTranslation.myForwardVector.y;
		listenerposition.OrientationFront.Z = aTranslation.myForwardVector.z;
		
		listenerposition.OrientationTop.X = aTranslation.myUpVector.x;
		listenerposition.OrientationTop.Y = aTranslation.myUpVector.y;
		listenerposition.OrientationTop.Z = aTranslation.myUpVector.z;

		myWwiseManager->SetListenerPosition(listenerposition);
	}

	void CAudioInterface::SetGameObjectPosition(const GameObjectID aGameObjectID, const CU::Vector3f & aPosition, const CU::Vector3f & aOrientation)
	{
		AkSoundPosition soundPosition;

		const CU::Vector3f position = aPosition * SoundSceneScale;

		soundPosition.Position.X = position.x;
		soundPosition.Position.Y = position.y;
		soundPosition.Position.Z = position.z;

		soundPosition.Orientation.X = aOrientation.x;
		soundPosition.Orientation.Y = aOrientation.y;
		soundPosition.Orientation.Z = aOrientation.z;

		myWwiseManager->SetGameObjectPosition(aGameObjectID, soundPosition);
	}

	void CAudioInterface::SetRTPCValue(const char * aRTPCName, const float aValue)
	{
		myWwiseManager->SetRTPC(aRTPCName, aValue);
	}

	GameObjectID CAudioInterface::RegisterGameObject()
	{
		GameObjectID newID = myGameObjectCounter;
		myWwiseManager->RegisterGameObject(newID);
		myGameObjectCounter++;

		return newID;
	}

	GameObjectID CAudioInterface::RegisterGameObject(const char * aGameObjectName)
	{
		GameObjectID newID = myGameObjectCounter;
		myWwiseManager->RegisterGameObject(newID, aGameObjectName);
		myGameObjectCounter++;

		return newID;
	}

	void CAudioInterface::UnregisterGameObject(const GameObjectID aGameObjectID)
	{
		myWwiseManager->UnregisterGameObject(aGameObjectID);
	}

	void CAudioInterface::UnregisterAllGameOBjects()
	{
		myWwiseManager->UnregisterAllGameObjects();
	}

	bool CAudioInterface::LoadBank(const char* aBankPath)
	{
		if (myWwiseManager)
		{
			return myWwiseManager->LoadBank(aBankPath);
		}
		return false;
	}

	void CAudioInterface::UnLoadBank(const char* aBankPath)
	{
		if (myWwiseManager)
		{
			return myWwiseManager->UnLoadBank(aBankPath);
		}
	}

	void CAudioInterface::PostEvent(const char* aEvent, const GameObjectID aGameObjectID)
	{
		if (myWwiseManager)
		{
			return myWwiseManager->PostEvent(aEvent, aGameObjectID);
		}
	}

	void CAudioInterface::PostEvent(const char* aEvent)
	{
		PostEvent(aEvent, myGameObjectID);
	}

	void CAudioInterface::SetErrorCallBack(callback_function aErrorCallback)
	{
		if (myWwiseManager)
		{
			return myWwiseManager->SetErrorCallBack(aErrorCallback);
		}
	}
}
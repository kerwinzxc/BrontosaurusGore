#include "stdafx.h"
#include "AudioSourceComponent.h"
#include "GameObject.h"
#include "ComponentManager.h"
#include "../Audio/AudioInterface.h"

CAudioSourceComponent::CAudioSourceComponent()
{
	myIsActive = true;
	myAudioInterface = Audio::CAudioInterface::GetInstance();
	myType = eComponentType::eAudioSource;
}


CAudioSourceComponent::~CAudioSourceComponent()
{
	myAudioInterface = nullptr;
}

void CAudioSourceComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::ePlaySound:
		PushEvent(aMessageData.myString);
		break;
	default:
		break;
	}
}

void CAudioSourceComponent::Update()
{
	if (myIsActive == true)
	{
		CU::Matrix44f& temp = GetParent()->GetToWorldTransform();
		myAudioInterface->SetGameObjectPosition(myGameObjectID, temp.GetPosition(), temp.myForwardVector);
	}
}

void CAudioSourceComponent::PushEvent(const char * aEvent)
{
	if (myIsActive == true)
	{
		myAudioInterface->PostEvent(aEvent, myGameObjectID);
	}
}

void CAudioSourceComponent::Destroy()
{
	DL_ASSERT("not implemented anymore, mvh carl");
	//GetParent()->GetComponents().RemoveCyclic(this);
	//CComponentManager::GetInstance().DeleteComponent(myGameObjectID);
}

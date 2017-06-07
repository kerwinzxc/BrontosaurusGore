#include "stdafx.h"
#include "GameObject.h"
#include "GameObjectManager.h"


CGameObject::CGameObject(CComponent* aNullComponent)
{
	myTransformId = -1;
	myComponents.Init(1);
	myComponents.Add(aNullComponent);
	aNullComponent->myParent = this;
	myManager = nullptr;
	myName = "null object";
}

CU::Matrix44f& CGameObject::GetLocalTransform()
{
	return myManager->GetTransform(*this);	
}

CU::Matrix44f CGameObject::GetToWorldTransform()
{
	if (GetParent() != nullptr)
	{
		const CU::Matrix44f parentTransform = GetParent()->GetToWorldTransform();
		const CU::Matrix44f localTransform = GetLocalTransform();
		return localTransform * parentTransform;
	}
	else
	{
		const CU::Matrix44f localTransform = GetLocalTransform();
		return localTransform;
	}
}

CU::Vector3f CGameObject::GetWorldPosition()
{
	if (myParent != nullptr)
	{
		return GetLocalTransform().GetPosition() + GetParent()->GetWorldPosition();
	}
	else
	{
		return GetLocalTransform().GetPosition();
	}
}

void CGameObject::SetWorldPosition(CU::Vector3f aPosition)
{
	if (GetParent() != nullptr)
	{
		GetParent()->SetWorldPosition(GetLocalTransform().GetPosition() + aPosition);
	}
	else
	{
		GetLocalTransform().SetPosition(aPosition);
	}
}

void CGameObject::SetWorldTransformation(const CU::Matrix44f & aTransformation)
{
	if (GetParent() != nullptr)
	{
		GetLocalTransform() = aTransformation * GetParent()->GetToWorldTransform().GetInverted();
	}
	else
	{
		GetLocalTransform() = aTransformation;
	}
}

void CGameObject::AddComponent(CComponent* aComponent)
{
	if (aComponent == nullptr)
	{
		DL_MESSAGE_BOX("trying to add null component to game object with id %u", GetId());
		return;
	}

	aComponent->myParent = this;
	myComponents.Add(aComponent);
	SComponentMessageData data;
	data.myComponentTypeAdded = aComponent->myType;
	data.myComponent = aComponent;
	Receive(eComponentMessageType::eAddComponent, data);
}

void CGameObject::RemoveComponent(CComponent * aComponent)
{
	myComponents.RemoveCyclic(aComponent);
}

void CGameObject::NotifyComponents(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	if (myParent != nullptr)
	{
		myParent->NotifyComponents(aMessageType, aMessageData);
	}
	else
	{
		Receive(aMessageType, aMessageData);
	}
}

void  CGameObject::NotifyOnlyComponents(const eComponentMessageType aMessageType, const SComponentMessageData &aMessageData)
{
	ComponentReceive(aMessageType, aMessageData);
}

bool CGameObject::AskComponents(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	if (myParent != nullptr)
	{
		return myParent->AskComponents(aQuestionType, aQuestionData);
	}
	else
	{
		return Answer(aQuestionType, aQuestionData);
	}
}

void CGameObject::MarkForDestruction()
{
	myManager->AddObjectForDestruction(this);
}

bool CGameObject::IsGameObject()
{
	return true;
}

void CGameObject::Move(const CU::Vector3f& aDispl)
{
	GetLocalTransform().Move(aDispl);
	NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
}

void CGameObject::Face(const CU::Vector3f& aDirection)
{
	LookAt(GetWorldPosition() + aDirection);
}

void CGameObject::LookAt(const CU::Vector3f& aDirection)
{
	GetLocalTransform().LookAt(aDirection);
	NotifyComponents(eComponentMessageType::eMoving, SComponentMessageData());
}

void CGameObject::Destroy()
{
	for (unsigned int i = 0; i < myComponents.Size(); ++i)
	{
		myComponents[i]->Destroy();
		myComponents[i] = nullptr;
	}

	myManager->DestroyObject(this);
}

void CGameObject::Receive(const eComponentMessageType aMessageType, const SComponentMessageData & aMessageData)
{
	for (unsigned short i = 0; i < myComponents.Size(); i++)
	{
		myComponents[i]->Receive(aMessageType, aMessageData);
	}
}

CU::GrowingArray<CComponent*>& CGameObject::GetComponents()
{
	return myComponents;
}

void CGameObject::ComponentReceive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	for (unsigned int i = 0; i < myComponents.Size(); i++)
	{
		if (myComponents[i]->IsGameObject() == false)
		{
			myComponents[i]->Receive(aMessageType, aMessageData);
		}
	}
}

bool CGameObject::Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData)
{
	for (CComponent* component : myComponents)
	{
		if (component->Answer(aQuestionType, aQuestionData) == true)
		{
			return true;
		}
	}
	return false;
}

CGameObject::CGameObject(CGameObjectManager &aManager)
	: myTransformId(0)
	, myManager(&aManager)
{
	myType = eComponentType::eGameObject;
	myComponents.Init(4);
}

CGameObject::~CGameObject()
{
}

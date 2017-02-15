#include "stdafx.h"
#include "FireComponent.h"
#include "..\BrontosaurusEngine\Scene.h"
#include "..\BrontosaurusEngine\FireEmitterInstance.h"


CFireComponent::CFireComponent(CScene& aScene, InstanceID aFireEmitterID)
	: myScene(aScene)
	, myID(aFireEmitterID)
{
}

CFireComponent::~CFireComponent()
{
}

void CFireComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	switch (aMessageType)
	{
	case eComponentMessageType::eObjectDone:
	case eComponentMessageType::eMoving:
		CGameObject* parent = GetParent();
		if (parent)
		{
			myScene.GetFireEmitter(myID).SetPosition(parent->GetWorldPosition());
		}
		break;
	}
}

void CFireComponent::Destroy()
{
}

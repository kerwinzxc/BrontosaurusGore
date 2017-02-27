#include "stdafx.h"
#include "ColliderComponentManager.h"
#include "BoxColliderComponent.h"
#include "ComponentManager.h"
#include "Component.h"

CColliderComponentManager::CColliderComponentManager()
{
	myColliderComponents.Init(128);
}

CColliderComponentManager::~CColliderComponentManager()
{
	myColliderComponents.DeleteAll();
}

CColliderComponent* CColliderComponentManager::CreateComponent(SColliderData* aColliderData)
{
	switch (aColliderData->myType)
	{
	case SColliderData::eColliderType::eBox:
		myColliderComponents.Add(new CBoxColliderComponent(reinterpret_cast<SBoxColliderData*>(aColliderData)));
		break;
	case SColliderData::eColliderType::eSphere:

		break;
	case SColliderData::eColliderType::eCapsule:

		break;
	case SColliderData::eColliderType::eMesh:

		break;
	default:
		break;
	}
	CComponentManager::GetInstance().RegisterComponent(myColliderComponents.GetLast());
	return myColliderComponents.GetLast();
}

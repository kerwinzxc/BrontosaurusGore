#include "stdafx.h"
#include "ModelComponent.h"
#include "AnimationComponent.h"

#include "../BrontosaurusEngine/ModelInstance.h"
#include "HighlightComponent.h"

CModelComponent::CModelComponent(CModelInstance& aModel)
	: myModel(aModel)
{
	myType = eComponentType::eModel;
}

CModelComponent::CModelComponent(CModelInstance& aModel, const bool aIsDebugSphere)
	: CModelComponent(aModel)
{
	myType = eComponentType::eModelDebug;
}

CModelComponent::~CModelComponent()
{
}

CU::Matrix44f CModelComponent::GetToWorldTransform()
{
	if (GetParent() != nullptr)
	{
		CU::Matrix44f worldTransform = GetParent()->GetToWorldTransform();

		return worldTransform;
	}
	else
	{
		return CU::Matrix44f::Identity;
	}
}

void CModelComponent::SetVisibility(const bool aVisibility)
{
	myModel.SetVisibility(aVisibility);
}

void CModelComponent::FlipVisibility()
{
	myModel.SetVisibility(!myModel.GetVisibility());
}

void CModelComponent::Receive(const eComponentMessageType aType, const SComponentMessageData &aData)
{
	switch (aType)
	{
	case eComponentMessageType::eAddComponent:
		if (aData.myComponentTypeAdded == eComponentType::eModel)
		{
			CreateAnimationComponent();
		}
		else if(aData.myComponentTypeAdded == eComponentType::eArmorPickup || 
			aData.myComponentTypeAdded == eComponentType::eAmmoPickup || 
			aData.myComponentTypeAdded == eComponentType::eHealthPickupComponent ||
				aData.myComponentTypeAdded == eComponentType::eKeyPickup)
		{
			myModel.SetHighlight(CU::Vector4f(0, 1, 0, 1), 0.75f);
		}
		else if(aData.myComponentTypeAdded == eComponentType::eHighlightComponent)
		{
			CHighlightComponent* component = static_cast<CHighlightComponent*>(aData.myComponent);

			myModel.SetHighlight(component->GetColor(), component->GetIntensity());
		}
	case eComponentMessageType::eMoving:
		myModel.SetTransformation(GetToWorldTransform());
		break;
	case eComponentMessageType::eSetVisibility:
		SetVisibility(aData.myBool);
		break;
	case eComponentMessageType::eSetHighlight:
		myModel.SetHighlightIntencity(aData.myFloat);
		break;
	case eComponentMessageType::eDied:
		myModel.SetVisibility(false);
		break;
	}
}

void CModelComponent::CreateAnimationComponent()
{
	if (myModel.myHasAnimations)
	{
		if (GetParent() && CComponentManager::GetInstancePtr() != nullptr)
		{
			CAnimationComponent* animationComponent = new CAnimationComponent(*this);
			CComponentManager::GetInstance().RegisterComponent(animationComponent);
			GetParent()->AddComponent(animationComponent);
		}
	}
}

void CModelComponent::SetAnimation(const std::string& aAnimationKey)
{
	myModel.SetAnimation(aAnimationKey);
}

void CModelComponent::SetNextAnimation(const std::string& aAnimationKey)
{
	myModel.SetNextAnimation(aAnimationKey);
}

void CModelComponent::SetAnimationLerpValue(const float aLerpValue)
{
	myModel.SetAnimationLerpie(aLerpValue);
}

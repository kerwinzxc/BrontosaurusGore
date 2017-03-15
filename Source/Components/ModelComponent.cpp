#include "stdafx.h"
#include "ModelComponent.h"
#include "AnimationComponent.h"

#include "../BrontosaurusEngine/ModelInstance.h"

CModelComponent::CModelComponent(CModelInstance& aModel)
	: myModel(aModel)
{
	myType = eComponentType::eModel;

	if (myModel.myHasAnimations)
	{

	}
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
	case eComponentMessageType::eMoving:
		myModel.SetTransformation(GetToWorldTransform());
		break;
	case eComponentMessageType::eSetVisibility:
		SetVisibility(aData.myBool);
		break;
	case eComponentMessageType::eSetHighlight:
		myModel.SetHighlightIntencity(aData.myFloat);
		break;
	}
}

void CModelComponent::ChangeAnimation(const char* aAnimationKey)
{
	myModel.ChangeAnimation(aAnimationKey);
}

#include "stdafx.h"
#include "ButtonAnimation.h"
#include "ModelWidget.h"
#include "WidgetFactory.h"

#include "..\BrontosaurusEngine\ModelInstance.h"
#include <CommonUtilities\Camera.h>
#include "CommonUtilities\Tween.h"

namespace GUI
{
	const float MilliSecondsPerFrame = 41.7f * .5f;

	ButtonAnimation::ButtonAnimation(IWidget* aModelWidget)
		: WidgetDecorator(aModelWidget, aModelWidget->GetLocalPosition(), aModelWidget->GetSize(), aModelWidget->GetName() + "_ButtonAnimation", aModelWidget->IsVisible())
		, myAnimationState(eAnimationState::eInActive)
		, myAnimationIsDoneCallback(nullptr)
		, myAnimationTimer(nullptr)
		, myTweener(nullptr)
		, myResetPosition(0.f)
		, myTurnPosition(6.266f)
		, myGoDownTime(0.62663f * 0.5f)
		, myGoUpTime(0.62663f * 0.5f)
	{
		ModelWidget* modelWidget = static_cast<ModelWidget*>(*myDecoratedWidget);
		if (modelWidget)
		{
			const CU::Matrix44f& transformation = modelWidget->myModelInstance->GetTransformation();
			myOriginalPosition = transformation.GetPosition();
			myResetPosition = myOriginalPosition.y;

			const CU::Camera* guiCamera = WidgetFactory::GetCurrentGUICamera();
			if (guiCamera)
			{
				myForwardDirection = myOriginalPosition - guiCamera->GetPosition();
				myForwardDirection.Normalize();
			}
		}

		myAnimationTimer = new CU::Time(0.f);
	}

	ButtonAnimation::~ButtonAnimation()
	{
		SAFE_DELETE(myAnimationTimer);
		SAFE_DELETE(myTweener);
	}

	void ButtonAnimation::Update(const CU::Time& aDeltaTime)
	{
		if (myTweener)
		{
			myTweener->Update(aDeltaTime.GetSeconds());
		}

		if (myAnimationState == eAnimationState::eStarted)
		{
			DoStartedAnimation(aDeltaTime);
		}
		else if (myAnimationState == eAnimationState::eFlipped)
		{
			DoFlippedAnimation(aDeltaTime);
		}
		else if (myAnimationState == eAnimationState::eDone)
		{
			DoDoneAnimation();
		}
	}

	void ButtonAnimation::OnMousePressed(const CU::Vector2f& aMousePosition, const CU::eMouseButtons aButton)
	{
		SUPRESS_UNUSED_WARNING(aMousePosition);
		if (aButton == CU::eMouseButtons::LBUTTON)
		{
			myAnimationState = eAnimationState::eStarted;
			if (myTweener)
			{
				SAFE_DELETE(myTweener);
			}

			myTweener = new CU::Tween(CU::TweenType::Bounce, CU::TweenMod::EaseIn, myResetPosition, myTurnPosition, myGoDownTime);
		}
	}

	void ButtonAnimation::OnMouseReleased(const CU::Vector2f& aMousePosition, const CU::eMouseButtons aButton)
	{
		SUPRESS_UNUSED_WARNING(aMousePosition);
		if (aButton == CU::eMouseButtons::LBUTTON)
		{
			myAnimationState = eAnimationState::eFlipped;

			if (myTweener)
			{
				SAFE_DELETE(myTweener);
			}

			myTweener = new CU::Tween(CU::TweenType::Bounce, CU::TweenMod::EaseOut, myResetPosition, myTurnPosition, myGoUpTime);
		}
	}

	void ButtonAnimation::OnMouseEnter(const CU::Vector2f& aMousePosition)
	{
		SUPRESS_UNUSED_WARNING(aMousePosition);
		ModelWidget* modelWidget = static_cast<ModelWidget*>(*myDecoratedWidget);
		if (modelWidget != nullptr)
		{
			modelWidget->SetFlashTimeToMax();
		}
	}

	void ButtonAnimation::DoStartedAnimation(const CU::Time aDeltaTime)
	{
		*myAnimationTimer += aDeltaTime;
		if (aDeltaTime.GetMilliseconds() < 60.f)
		{
			ModelWidget* modelWidget = static_cast<ModelWidget*>(*myDecoratedWidget);
			if (modelWidget != nullptr)
			{
				CU::Matrix44f transformation = modelWidget->myModelInstance->GetTransformation();
				CU::Vector3f& pos = transformation.GetPosition();
				if (myTweener)
				{
					pos += myForwardDirection * myTweener->GetValue() * aDeltaTime.GetMilliseconds();
				}
				else
				{
					pos += myForwardDirection * aDeltaTime.GetMilliseconds() * 0.1f;
				}

				modelWidget->myModelInstance->SetTransformation(transformation);
			}
		}


		if (myAnimationTimer->GetMilliseconds() > MilliSecondsPerFrame * 3.f)
		{
			myAnimationState = eAnimationState::ePaused;
			myAnimationTimer->Reset();
		}
	}

	void ButtonAnimation::DoFlippedAnimation(const CU::Time aDeltaTime)
	{
		ModelWidget* modelWidget = static_cast<ModelWidget*>(*myDecoratedWidget);
		if (modelWidget == nullptr) return;

		*myAnimationTimer += aDeltaTime;
		CU::Matrix44f transformation = modelWidget->myModelInstance->GetTransformation();

		CU::Vector3f& pos = transformation.GetPosition();
		if (myTweener)
		{
			pos += myForwardDirection * myTweener->GetValue() * -1.f * aDeltaTime.GetMilliseconds();
		}
		else
		{
			pos += myForwardDirection * aDeltaTime.GetMilliseconds() * -0.1f;
		}

		if (pos.y >= myResetPosition)
		{
			pos.y = myResetPosition;
			myAnimationState = eAnimationState::eDone;
			myAnimationTimer->Reset();
		}

		modelWidget->myModelInstance->SetTransformation(transformation);
	}

	void ButtonAnimation::DoDoneAnimation()
	{
		myAnimationState = eAnimationState::eInActive;
		myAnimationTimer->Reset();


		if (myAnimationIsDoneCallback != nullptr)
		{
			myAnimationIsDoneCallback();
		}
	}
}

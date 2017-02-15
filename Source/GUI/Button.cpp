#include "stdafx.h"
#include "Button.h"

#include "ModelWidget.h"
#include "ButtonAnimation.h"

#include "../BrontosaurusEngine/ModelInstance.h"
#include "../Audio/AudioInterface.h"

namespace GUI
{
	Button::Button(const std::function<void(void)>& aFunction, const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const std::string& aName)
		: WidgetContainer(aPosition, aSize, aName, true)
		, myFunction(aFunction)
		, myWasClicked(false)
	{
	}

	Button::~Button()
	{
	}

	void Button::AddWidget(const std::string& aWidgetName, IWidget* aWidget)
	{
		WidgetContainer::AddWidget(aWidgetName, aWidget);

		if (aWidgetName == "Animation")
		{
			ButtonAnimation* buttonAnimationWidget = static_cast<ButtonAnimation*>(*aWidget);
			if (buttonAnimationWidget == nullptr)
			{
				assert(!"Got widget called Animation that was not a ButtonAnimation widget");
				return;
			}

			buttonAnimationWidget->SetCallbackFunction(std::bind(&Button::OnAnimationDone, this));
		}
	}

	IWidget* Button::MouseIsOver(const CU::Vector2f & aPosition)
	{
		return IWidget::MouseIsOver(aPosition);
	}

	void Button::OnMousePressed(const CU::Vector2f& aMousePosition, CU::eMouseButtons aButton)
	{
		for (IWidget* child : myOrderedWidgets)
		{
			child->OnMousePressed(aMousePosition, aButton);
		}

		if (aButton == CU::eMouseButtons::LBUTTON)
		{
			Audio::CAudioInterface::GetInstance()->PostEvent("ButtonClick");
			myWasClicked = true;
		}
	}

	void Button::OnMouseReleased(const CU::Vector2f& aMousePosition, const CU::eMouseButtons aButton)
	{
		//WidgetContainer::OnMouseReleased(aMousePosition, aButton);
		for (IWidget* child : myOrderedWidgets)
		{
			child->OnMouseReleased(aMousePosition, aButton);
		}

		if (aButton == CU::eMouseButtons::LBUTTON)
		{
			if (IsInside(aMousePosition) == false)
			{
				myWasClicked = false;
			}
			else if (FindWidget("Animation") == nullptr)
			{
				RunCallbackFunction();
				myWasClicked = false;
			}
		}
	}

	void Button::OnMouseMove(const CU::Vector2f& aMousePosition)
	{
		WidgetContainer::OnMouseMove(aMousePosition);
	}

	void Button::OnMouseEnter(const CU::Vector2f& aMousePosition)
	{
		Audio::CAudioInterface::GetInstance()->PostEvent("ButtonHover");
		WidgetContainer::OnMouseEnter(aMousePosition);
	}

	void Button::OnMouseExit(const CU::Vector2f& aMousePosition)
	{
		WidgetContainer::OnMouseExit(aMousePosition);
	}

	void Button::OnAnimationDone()
	{
		if (myWasClicked == true)
		{
			RunCallbackFunction();
			myWasClicked = false;
		}
	}

	void Button::RunCallbackFunction()
	{
		if (myWasClicked == true && myFunction != nullptr)
		{
			myFunction();
		}
	}
}

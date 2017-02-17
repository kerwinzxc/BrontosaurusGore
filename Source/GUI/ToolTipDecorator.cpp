#include "stdafx.h"
#include "ToolTipDecorator.h"

#include "../BrontosaurusEngine/TextInstance.h"
#include "ModelWidget.h"
#include "BrontosaurusEngine/SpriteInstance.h"

namespace GUI
{
	CToolTipDecorator::CToolTipDecorator(IWidget* aDecoratedWidget, ModelWidget* /*aBackGround*/, const std::string* const aTooltipText, const std::function<bool(std::string&)>& aGetTextFunction)
		: WidgetDecorator(aDecoratedWidget, CU::Vector2f::Zero, CU::Vector2f::Zero, aDecoratedWidget->GetName(), false)
		, myGetTextFunction(aGetTextFunction)
		, myOffsetToMouse(0.f, -0.05f)
		, myBackGround(nullptr)
		, myTextInstance(nullptr)
		, myShouldRender(false)
	{
		myTextInstance = new CTextInstance();
		myTextInstance->Init();
		myTextInstance->SetPosition(aDecoratedWidget->GetWorldPosition());
		if (aTooltipText != nullptr)
		{
			myTextInstance->SetText(aTooltipText->c_str());
		}
		myUpdateTextfunctionTimer.Init();
		myBackGround = new CSpriteInstance("Sprites/tooltipBackground.dds", myTextInstance->GetQuadSizeNormalized(), aDecoratedWidget->GetWorldPosition());
	}

	CToolTipDecorator::~CToolTipDecorator()
	{
		SAFE_DELETE(myBackGround);
		SAFE_DELETE(myTextInstance);
	}

	void CToolTipDecorator::OnMouseEnter(const CU::Vector2f& aMousePosition)
	{
		WidgetDecorator::OnMouseEnter(aMousePosition);

		if (myGetTextFunction != nullptr)
		{
			std::string updatedTooltipText("");
			if (myGetTextFunction(updatedTooltipText))
			{
				myTextInstance->SetText(updatedTooltipText.c_str());
			}
		}

		myShouldRender = true;
		CU::Vector2f backGroundSize = myTextInstance->GetQuadSizeNormalized();

		CU::Vector2f newPosition(aMousePosition + myOffsetToMouse);
		myTextInstance->SetPosition(newPosition + CU::Vector2f(0.f, backGroundSize.y * 0.5f));
		myBackGround->SetPosition(newPosition);

		myBackGround->SetSize(backGroundSize * CU::Vector2f(1.16f, 1.2f));
	}

	void CToolTipDecorator::OnMouseExit(const CU::Vector2f& aMousePosition)
	{
		WidgetDecorator::OnMouseExit(aMousePosition);
		
		myShouldRender = false;
		myBackGround->SetPosition(aMousePosition + myOffsetToMouse);
		myTextInstance->SetPosition(aMousePosition + myOffsetToMouse);
	}

	GUI::IWidget* CToolTipDecorator::MouseIsOver(const CU::Vector2f& aPosition)
	{
		IWidget* mouseIsOver = WidgetDecorator::MouseIsOver(aPosition);
		if (mouseIsOver == myDecoratedWidget)
		{
			return this;
		}

		return mouseIsOver;
	}

	void CToolTipDecorator::SetTextFunction(const std::function<bool(std::string&)>& aGetTextFunction)
	{
		assert(myGetTextFunction == nullptr);
		myGetTextFunction = aGetTextFunction;
	}

	void CToolTipDecorator::ChangeTextFunction(const std::function<bool(std::string&)>& aGetTextFunction)
	{
		//assert(myGetTextFunction != nullptr);
		myGetTextFunction = aGetTextFunction;
	}

	void CToolTipDecorator::Update(const CU::Time& /*aDeltaTime*/)
	{
		if (myGetTextFunction)
		{
			myUpdateTextfunctionTimer.Update();
			if (myUpdateTextfunctionTimer.GetLifeTime().GetMilliseconds() > 500.f)
			{
				myUpdateTextfunctionTimer.Restart();

				std::string updatedTooltipText("");
				if (myGetTextFunction(updatedTooltipText))
				{
					myTextInstance->SetText(updatedTooltipText.c_str());
				}
			}
		}
	}

	void CToolTipDecorator::Render()
	{
		//WidgetDecorator::Render();

		if (myShouldRender == true)
		{
			myBackGround->Render();
			myTextInstance->Render();
		}
	}

	void CToolTipDecorator::Render(CU::GrowingArray<IWidget*>& aWidgets)
	{
		WidgetDecorator::Render(aWidgets);
		aWidgets.Add(this);
	}

	void CToolTipDecorator::OnMouseMove(const CU::Vector2f& aMousePosition)
	{
		CU::Vector2f backGroundSize = myTextInstance->GetQuadSizeNormalized();
		CU::Vector2f newPosition(aMousePosition + myOffsetToMouse);

		myBackGround->SetPosition(newPosition);
		myTextInstance->SetPosition(newPosition + CU::Vector2f(0.f, backGroundSize.y * 0.5f));

		myBackGround->SetSize(backGroundSize * CU::Vector2f(1.16f, 1.2f));
	}
}

#include "stdafx.h"
#include "WidgetDecorator.h"

namespace GUI
{
	WidgetDecorator::WidgetDecorator(IWidget* aWidget, const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const std::string& aName, const bool aIsVisible)
		: IWidget(aPosition, aSize, aName, aIsVisible), myDecoratedWidget(aWidget)
	{
	}

	WidgetDecorator::~WidgetDecorator()
	{
		SAFE_DELETE(myDecoratedWidget);
	}

	void WidgetDecorator::Update(const CU::Time& aDeltaTime)
	{
		myDecoratedWidget->Update(aDeltaTime);
	}

	void WidgetDecorator::Render()
	{
		myDecoratedWidget->Render();
	}

	void WidgetDecorator::Render(CU::GrowingArray<IWidget*>& aFrontWidgets)
	{
		myDecoratedWidget->Render(aFrontWidgets);
	}

	void WidgetDecorator::OnMouseMove(const CU::Vector2f& aPosition)
	{
		myDecoratedWidget->OnMouseMove(aPosition);
	}

	void WidgetDecorator::OnMousePressed(const CU::Vector2f& aPosition, const CU::eMouseButtons aButton)
	{
		myDecoratedWidget->OnMousePressed(aPosition, aButton);
	}

	void WidgetDecorator::OnMouseDown(const CU::Vector2f& aPosition, const CU::eMouseButtons aButton)
	{
		myDecoratedWidget->OnMouseDown(aPosition, aButton);
	}

	void WidgetDecorator::OnMouseReleased(const CU::Vector2f& aPosition, const CU::eMouseButtons aButton)
	{
		myDecoratedWidget->OnMouseReleased(aPosition, aButton);
	}

	void WidgetDecorator::OnMouseEnter(const CU::Vector2f& aMousePosition)
{
		myDecoratedWidget->OnMouseEnter(aMousePosition);
	}

	void WidgetDecorator::OnMouseExit(const CU::Vector2f& aMousePosition)
{
		myDecoratedWidget->OnMouseExit(aMousePosition);
	}

	void WidgetDecorator::SetSize(const CU::Vector2f& aSize)
	{
		myDecoratedWidget->SetSize(aSize);
	}

	void WidgetDecorator::SetVisibility(const bool aVisibility)
	{
		myDecoratedWidget->SetVisibility(aVisibility);
	}

	IWidget* WidgetDecorator::FindWidget(const std::string& aWidgetName)
	{
		return myDecoratedWidget->FindWidget(aWidgetName);
	}

	void WidgetDecorator::OnPositionChanged()
	{
		myDecoratedWidget->SetLocalPosition(GetLocalPosition());
	}

	const std::string& WidgetDecorator::GetName() const
	{
		//return myDecoratedWidget->GetName();
		return IWidget::GetName();
	}

	CU::Vector2f WidgetDecorator::GetWorldPosition() const
	{
		return myDecoratedWidget->GetWorldPosition();
	}

	const CU::Vector2f & WidgetDecorator::GetLocalPosition() const
	{
		return myDecoratedWidget->GetLocalPosition();
	}

	const CU::Vector2f & WidgetDecorator::GetSize() const
	{
		return myDecoratedWidget->GetSize();
	}

	bool WidgetDecorator::IsVisible() const
	{
		return myDecoratedWidget->IsVisible();
	}

	void WidgetDecorator::OnMouseDrag(const CU::Vector2f& aDisplacement, const CU::eMouseButtons aButton)
	{
		myDecoratedWidget->OnMouseDrag(aDisplacement, aButton);
	}

	void WidgetDecorator::OnGotFocus()
	{
		myDecoratedWidget->OnGotFocus();
	}

	void WidgetDecorator::OnLostFocus()
	{
		myDecoratedWidget->OnLostFocus();
	}

	GUI::IWidget* WidgetDecorator::MouseIsOver(const CU::Vector2f& aPosition)
	{
		return myDecoratedWidget->MouseIsOver(aPosition);
	}

	void WidgetDecorator::SetWorldPosition(const CU::Vector2f& aPosition)
	{
		myDecoratedWidget->SetWorldPosition(aPosition);
	}

	void WidgetDecorator::SetLocalPosition(const CU::Vector2f& aPosition)
	{
		myDecoratedWidget->SetLocalPosition(aPosition);
	}

	void WidgetDecorator::SetVisibilityRecursive(const bool aVisibility)
	{
		myDecoratedWidget->SetVisibilityRecursive(aVisibility);
	}

	void WidgetDecorator::SetName(const std::string& aName)
	{
		myDecoratedWidget->SetName(aName);
	}

	bool WidgetDecorator::HasDebugLines() const
	{
		return myDecoratedWidget->HasDebugLines();
	}

	void WidgetDecorator::RemoveDebugLines() const
	{
		myDecoratedWidget->RemoveDebugLines();
	}
}

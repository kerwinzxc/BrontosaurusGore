#include "stdafx.h"
#include "Widget.h"

#include "../BrontosaurusEngine/Engine.h"
#include "../BrontosaurusEngine/LineDrawer.h"

#ifdef _DEBUG
#define _USE_LINE_DRAWER
#endif // _DEBUG

namespace GUI
{
	int IWidget::ourNamelessCount = 0;

	struct IWidget::WidgetImpl
	{
		std::string myName;
#ifdef _USE_LINE_DRAWER
		LineHandle myLineHandles[4];
#endif // _USE_LINE_DRAWER
		CU::Vector2f myPosition;
		CU::Vector2f mySize;
		IWidget* myParent;
		bool myIsVisible;
	};

	IWidget::IWidget(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const bool aIsVisible)
		: IWidget(aPosition, aSize, std::string("NamelessWidget") += std::to_string(++ourNamelessCount), aIsVisible)
	{
	}

	IWidget::IWidget(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const std::string& aName, const bool aIsVisible)
	{
		myImplementation = new WidgetImpl();
		myImplementation->myName = aName;
		myImplementation->myPosition = aPosition;
		myImplementation->mySize = aSize;
		myImplementation->myParent = nullptr;
		myImplementation->myIsVisible = aIsVisible;

#ifdef _USE_LINE_DRAWER
		memset(myImplementation->myLineHandles, 0, 4);
#endif // _USE_LINE_DRAWER
	}

	IWidget::~IWidget()
	{
		RemoveDebugLines();
		SAFE_DELETE(myImplementation);
	}

	IWidget* IWidget::FindWidget(const std::string& aWidgetName)
	{
		if (GetName() == aWidgetName)
		{
			return this;
		}

		return nullptr;
	}

	IWidget* IWidget::MouseIsOver(const CU::Vector2f& aPosition)
	{
		if (IsInside(aPosition) == true)
		{
			return this;
		}

		return nullptr;
	}

	void IWidget::SetWorldPosition(const CU::Vector2f& aPosition)
	{
		if (GetParent() == nullptr)
		{
			SetLocalPosition(aPosition);
		}
		else
		{
			SetLocalPosition(aPosition - GetParent()->GetWorldPosition());
		}
	}

	void IWidget::SetLocalPosition(const CU::Vector2f& aPosition)
	{
		CU::Vector2f displacement = aPosition - myImplementation->myPosition;
		myImplementation->myPosition = aPosition;
		OnPositionChanged();
		OnPositionChanged(displacement);
	}

	void IWidget::SetSize(const CU::Vector2f& aSize)
	{
		myImplementation->mySize = aSize;
	}

	void IWidget::SetVisibility(const bool aVisibility)
	{
		myImplementation->myIsVisible = aVisibility;
		
		//if (aVisibility == false)
		//{
		//	RemoveDebugLines();
		//}
	}

	void IWidget::SetVisibilityRecursive(const bool aVisibility)
	{
		myImplementation->myIsVisible = aVisibility;
	}

	void IWidget::SetName(const std::string& aName)
	{
		myImplementation->myName = aName;
	}

	CU::Vector2f IWidget::GetWorldPosition() const
	{
		if (GetParent() == nullptr)
		{
			return GetLocalPosition();
		}
		else
		{
			return GetParent()->GetWorldPosition() + GetLocalPosition();
		}
	}

	const CU::Vector2f& IWidget::GetLocalPosition() const
	{
		return myImplementation->myPosition;
	}

	const CU::Vector2f& IWidget::GetSize() const
	{
		return myImplementation->mySize;
	}

	bool IWidget::IsVisible() const
	{
		return myImplementation->myIsVisible;
	}

	bool IWidget::HasDebugLines() const
	{
#ifdef _USE_LINE_DRAWER
		const LineHandle NullHandles[] = { 0, 0, 0, 0 };

		if (memcmp(myImplementation->myLineHandles, NullHandles, sizeof(NullHandles)) == 0)
		{
			return false;
		}
#endif // _USE_LINE_DRAWER
		
		return true;
	}

	const std::string& IWidget::GetName() const
	{
		return myImplementation->myName;
	}

	void IWidget::SetParent(IWidget* aParent)
	{
		myImplementation->myParent = aParent;
	}

	void IWidget::AddDebugLines() const
	{
#ifdef _USE_LINE_DRAWER
		ENGINE->GetLineDrawer().AddQuad(GetWorldPosition(), GetWorldPosition() + GetSize(), myImplementation->myLineHandles);
#endif // _USE_LINE_DRAWER
	}

	void IWidget::RemoveDebugLines() const
	{
#ifdef _USE_LINE_DRAWER
		const LineHandle NullHandles[] = { 0, 0, 0, 0 };

		if (memcmp(myImplementation->myLineHandles, NullHandles, sizeof(NullHandles)) != 0)
		{
			for (int i = 0; i < 4; ++i)
			{
				ENGINE->GetLineDrawer().RemoveLine2D(myImplementation->myLineHandles[i]);
			}

			memcpy(myImplementation->myLineHandles, NullHandles, sizeof(NullHandles));
		}
#endif // _USE_LINE_DRAWER
	}

	IWidget* IWidget::GetParent()
	{
		return myImplementation->myParent;
	}

	const IWidget* IWidget::GetParent() const
	{
		return myImplementation->myParent;
	}

	void IWidget::SetChild(IWidget* aChild)
	{
		aChild->SetParent(this);
	}

	bool IWidget::IsInside(const CU::Vector2f& aPosition) const
	{
		if (aPosition.x < myImplementation->myPosition.x)
		{
			return false;
		}
		else if (aPosition.y < myImplementation->myPosition.y)
		{
			return false;
		}
		else if (aPosition.x >= myImplementation->myPosition.x + myImplementation->mySize.x)
		{
			return false;
		}
		else if (aPosition.y >= myImplementation->myPosition.y + myImplementation->mySize.y)
		{
			return false;
		}

		return true;
	}
}

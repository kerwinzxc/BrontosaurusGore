#include "stdafx.h"
#include "BarWidget.h"
#include "ModelWidget.h"

#include "GUIPixelConstantBuffer.h"

namespace GUI
{
	BarWidget::BarWidget(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const std::string& aName, const bool aIsVisible, const int aMaxValue, const int aStartValue)
		: WidgetContainer(aPosition, aSize, aName, aIsVisible)
		, myCachedChild(nullptr)
		, myProgressValue(aStartValue)
		, myMaxValue(aMaxValue)
		, myMillisecondsLeftSinceMouseEnter(0.f)
	{
	}

	BarWidget::~BarWidget()
	{
	}

	//void BarWidget::ValueChanged(const int aValue)
	//{
	//	myProgressValue = aValue;

	//	if (myProgressValue < myMaxValue)
	//	{
	//		SetVisibility(true);
	//		
	//		if (HasDebugLines() == false)
	//		{
	//			//AddDebugLines();
	//		}
	//		
	//		IWidget* widget = GetModelChild();
	//		if (widget != nullptr)
	//		{
	//			ModelWidget* modelWidget = static_cast<ModelWidget*>(widget);

	//			modelWidget->GetPixelConstantBufferStruct().myValues[SPixelConstantBuffer::eMember::eIsProgressBar] = 1.f;
	//			modelWidget->GetPixelConstantBufferStruct().myValues[SPixelConstantBuffer::eMember::eProgressBarValue] = 1.f - static_cast<float>(myProgressValue) / static_cast<float>(myMaxValue);
	//		}
	//	}
	//	else
	//	{
	//		FlashAndRemove();
	//	}
	//}

	//void BarWidget::SetMaxValue(const int aValue)
	//{
	//	myMaxValue = aValue;
	//}

	void BarWidget::Update(const CU::Time& aDeltaTime)
	{
		WidgetContainer::Update(aDeltaTime);

		IWidget* widget = GetModelChild();
		if (widget != nullptr)
		{
			ModelWidget* modelWidget = static_cast<ModelWidget*>(widget);
			if (myProgressValue >= myMaxValue && modelWidget->IsFlashing() == false)
			{
				//SetVisibility(false);
			}
		}
	}

	void BarWidget::FlashAndRemove()
	{
		IWidget* widget = GetModelChild();
		if (widget != nullptr)
		{
			ModelWidget* modelWidget = static_cast<ModelWidget*>(widget);
			modelWidget->SetFlashTimeToMax();

			assert(!"this is not supported anymore due to optimized values sent in gui constant buffer");
			//modelWidget->GetPixelConstantBufferStruct().myValues[SPixelConstantBuffer::eMember::eIsProgressBar] = 1.f;
			//modelWidget->GetPixelConstantBufferStruct().myValues[SPixelConstantBuffer::eMember::eProgressBarValue] = 0.f;
		}
	}

	IWidget* BarWidget::GetModelChild()
	{
		if (myCachedChild == nullptr)
		{
			myCachedChild = FindWidget("model");
		}

		return myCachedChild;
	}
}

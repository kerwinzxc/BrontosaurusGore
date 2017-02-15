#include "stdafx.h"
#include "TextWidget.h"

namespace GUI
{
	TextWidget::TextWidget(IWidget* aWidget, const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const std::string& aName, const std::string& /*aText*/, const bool aIsVisible)
		: WidgetDecorator(aWidget, aPosition, aSize, aName, aIsVisible)
	{
	}

	TextWidget::~TextWidget()
	{
	}
}

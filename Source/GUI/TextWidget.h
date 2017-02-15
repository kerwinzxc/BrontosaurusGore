#pragma once
#include "WidgetDecorator.h"

namespace GUI
{
	class TextWidget : public WidgetDecorator
	{
	public:
		TextWidget(IWidget* aWidget, const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const std::string& aName, const std::string& aText, const bool aIsVisible = true);
		~TextWidget();

	private:
		
	};
}

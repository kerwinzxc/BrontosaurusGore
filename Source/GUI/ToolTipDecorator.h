#pragma once

#include "WidgetDecorator.h"
#include "../CommonUtilities/StopWatch.h"

class CTextInstance;
class CSpriteInstance;

namespace GUI
{
	class ModelWidget;
	class CToolTipDecorator : public WidgetDecorator
	{
	public:
		CToolTipDecorator(IWidget* aDecoratedWidget, ModelWidget* aBackGround, const std::string* const aTooltipText, const std::function<bool(std::string&)>& aGetTextFunction);
		~CToolTipDecorator();

		void Update(const CU::Time& aDeltaTime) override;
		void Render() override;
		void Render(CU::GrowingArray<IWidget*>& aWidgets) override;
		void OnMouseMove(const CU::Vector2f& aMousePosition) override;
		void OnMouseEnter(const CU::Vector2f& aMousePosition) override;
		void OnMouseExit(const CU::Vector2f& aMousePosition) override;

		IWidget* MouseIsOver(const CU::Vector2f& aPosition) override;

		void SetTextFunction(const std::function<bool(std::string&)>& aGetTextFunction);
		void ChangeTextFunction(const std::function<bool(std::string&)>& aGetTextFunction);

		virtual operator CToolTipDecorator*() { return this; }

	private:
		std::function<bool(std::string&)> myGetTextFunction;
		CU::CStopWatch myUpdateTextfunctionTimer;
		CU::Vector2f myOffsetToMouse;
		CSpriteInstance* myBackGround;
		CTextInstance* myTextInstance;
		bool myShouldRender;
	};
}
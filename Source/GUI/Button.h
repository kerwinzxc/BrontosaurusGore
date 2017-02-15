#pragma once
#include "WidgetContainer.h"
#include <functional>

namespace GUI
{
	class Button : public WidgetContainer
	{
	public:
		Button(const std::function<void(void)>& aFunction, const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const std::string& aName);
		~Button();

		void AddWidget(const std::string& aWidgetName, IWidget* aWidget) override;
		IWidget* MouseIsOver(const CU::Vector2f& aPosition) override;

		void OnMousePressed(const CU::Vector2f& aMousePosition, const CU::eMouseButtons aButton) override;
		void OnMouseReleased(const CU::Vector2f& aMousePosition, const CU::eMouseButtons aButton) override;

		void OnMouseMove(const CU::Vector2f& aMousePosition) override;
		void OnMouseEnter(const CU::Vector2f& aMousePosition) override;
		void OnMouseExit(const CU::Vector2f& aMousePosition) override;

	private:
		void OnAnimationDone();
		void RunCallbackFunction();

		std::function<void(void)> myFunction;
		bool myWasClicked;
	};
}

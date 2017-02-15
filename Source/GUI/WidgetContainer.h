#pragma once
#include "Widget.h"
#include <unordered_map>

namespace GUI
{
	class WidgetContainer : public IWidget
	{
	public:

		WidgetContainer(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const bool aIsVisible);
		WidgetContainer(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const std::string& aName, const bool aIsVisible);
		virtual ~WidgetContainer();

		virtual void AddWidget(const std::string& aWidgetName, IWidget* aWidget);
		IWidget* RemoveWidget(const std::string& aWidgetName);
		IWidget* FindWidget(const std::string& aWidgetName) override;

		virtual void Update(const CU::Time& aDeltaTime) override;
		virtual void Render() override;
		virtual void Render(CU::GrowingArray<IWidget*>& aWidgets) override;
		virtual void SetVisibilityRecursive(const bool aVisibility) override;
		virtual void RemoveDebugLines() const override;

		virtual IWidget* MouseIsOver(const CU::Vector2f& aPosition) override;

		void RenderFrontLayers();
		void MoveToFront(const std::string& aWidgetName);
		void MoveToBack(const std::string& aWidgetName);

		inline virtual bool IsEmpty() const;

	protected:
		std::unordered_map<std::string, IWidget*> myWidgets;
		CU::GrowingArray<IWidget*> myOrderedWidgets;
		CU::GrowingArray<IWidget*> myFrontLayerWidgets;
	};

	inline bool WidgetContainer::IsEmpty() const
	{
		return myWidgets.empty();
	}
}

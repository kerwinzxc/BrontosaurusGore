#pragma once

#include "../CommonUtilities/vector2.h"

#ifndef SUPRESS_UNUSED_WARNING
#define SUPRESS_UNUSED_WARNING(variable) variable
#endif // !SUPRESS_UNUSED_WARNING

namespace CU
{
	class Time;
	class DynamicString;

	template <typename T>
	class Vector2;
	using Vector2f = Vector2<float>;

	enum class eMouseButtons;
}

namespace GUI
{
	class ModelWidget;
	class ButtonAnimation;
	class CToolTipDecorator;

	class IWidget
	{
	public:
		IWidget(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const bool aIsVisible);
		IWidget(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const std::string& aName, const bool aIsVisible);
		virtual ~IWidget();

		virtual void Update(const CU::Time& aDeltaTime) { SUPRESS_UNUSED_WARNING(aDeltaTime); }
		virtual void Render() {}
		virtual void Render(CU::GrowingArray<IWidget*>& aWidgets) { SUPRESS_UNUSED_WARNING(aWidgets); Render(); }

		virtual void OnMouseMove(const CU::Vector2f& aMousePosition) { SUPRESS_UNUSED_WARNING(aMousePosition); }
		virtual void OnMouseDrag(const CU::Vector2f& aDisplacement, const CU::eMouseButtons aButton) { SUPRESS_UNUSED_WARNING(aDisplacement); SUPRESS_UNUSED_WARNING(aButton); }
		virtual void OnMousePressed(const CU::Vector2f& aMousePosition, const CU::eMouseButtons aButton) { SUPRESS_UNUSED_WARNING(aMousePosition); SUPRESS_UNUSED_WARNING(aButton); }
		virtual void OnMouseDown(const CU::Vector2f& aMousePosition, const CU::eMouseButtons aButton) { SUPRESS_UNUSED_WARNING(aMousePosition); SUPRESS_UNUSED_WARNING(aButton); }
		virtual void OnMouseReleased(const CU::Vector2f& aMousePosition, const CU::eMouseButtons aButton) { SUPRESS_UNUSED_WARNING(aMousePosition); SUPRESS_UNUSED_WARNING(aButton); }
		virtual void OnMouseEnter(const CU::Vector2f& aMousePosition) { SUPRESS_UNUSED_WARNING(aMousePosition); }
		virtual void OnMouseExit(const CU::Vector2f& aMousePosition) { SUPRESS_UNUSED_WARNING(aMousePosition); }
		virtual void OnGotFocus() {}
		virtual void OnLostFocus() {}

		virtual IWidget* FindWidget(const std::string& aWidgetName);
		virtual IWidget* MouseIsOver(const CU::Vector2f& aMousePosition);

		virtual void OnPositionChanged() {}
		virtual void OnPositionChanged(const CU::Vector2f aDisplaceMent) { SUPRESS_UNUSED_WARNING(aDisplaceMent); }

		virtual void SetWorldPosition(const CU::Vector2f& aPosition);
		virtual void SetLocalPosition(const CU::Vector2f& aPosition);
		virtual void SetSize(const CU::Vector2f& aSize);
		virtual void SetVisibility(const bool aVisibility);
		virtual void SetVisibilityRecursive(const bool aVisibility);
		virtual void SetName(const std::string& aName);

		virtual CU::Vector2f GetWorldPosition() const;
		virtual const CU::Vector2f& GetLocalPosition() const;
		virtual const CU::Vector2f& GetSize() const;
		virtual bool IsVisible() const;
		virtual bool HasDebugLines() const;
		virtual const std::string& GetName() const;

		virtual void RemoveDebugLines() const;

		operator IWidget*() { return this; }
		virtual operator ModelWidget*() { return nullptr; }
		virtual operator ButtonAnimation*() { return nullptr; }
		virtual operator CToolTipDecorator*() { return nullptr; }

	protected:
		void AddDebugLines() const;

		IWidget* GetParent();
		const IWidget* GetParent() const;

		void SetChild(IWidget* aChild);
		void SetParent(IWidget* aParent);

		bool IsInside(const CU::Vector2f& aPosition) const;

	private:
		struct WidgetImpl;
		WidgetImpl* myImplementation;

		static int ourNamelessCount;
	};
}

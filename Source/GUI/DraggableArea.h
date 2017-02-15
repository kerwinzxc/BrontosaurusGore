#pragma once
#include "Widget.h"

namespace GUI
{
	class DraggableArea : public IWidget
	{
	public:
		DraggableArea(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, IWidget* aParent, const bool aIsVisible = false);
		~DraggableArea();

		void OnMouseDrag(const CU::Vector2f& aDisplacement, const CU::eMouseButtons aButton) override;
	};
}

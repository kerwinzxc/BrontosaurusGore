#include "stdafx.h"
#include "DraggableArea.h"

using namespace GUI;

DraggableArea::DraggableArea(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, IWidget* aParent, const bool aIsVisible)
	: IWidget(aPosition, aSize, aParent->GetName() + "DragArea", aIsVisible)
{
	SetParent(aParent);
}

DraggableArea::~DraggableArea()
{
}

void DraggableArea::OnMouseDrag(const CU::Vector2f& aDisplacement, const CU::eMouseButtons aButton)
{
	if (aButton == CU::eMouseButtons::LBUTTON)
	{
		SetLocalPosition(GetLocalPosition() + aDisplacement);
		GetParent()->SetLocalPosition(GetParent()->GetLocalPosition() + aDisplacement);
	}
}

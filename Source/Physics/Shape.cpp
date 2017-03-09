#include "stdafx.h"
#include "Shape.h"
#include <PxShape.h>

namespace Physics
{
	CShape::CShape(physx::PxShape* aShape)
	{
		myShape = aShape;
	}

	CShape::CShape(const CShape& aShape)
	{
		myShape = aShape.myShape;
	}

	CShape::~CShape()
	{
		if (myShape != nullptr)
			//myShape->release();
		myShape = nullptr;
	}
}
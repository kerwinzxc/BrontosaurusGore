#include "stdafx.h"
#include "Shape.h"
#include <PxShape.h>


CShape::CShape(physx::PxShape* aShape)
{
	myShape = aShape;
}

CShape::~CShape()
{
	if(myShape != nullptr)
		myShape->release();
	myShape = nullptr;
}

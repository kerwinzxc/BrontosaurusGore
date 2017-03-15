#include "stdafx.h"
#include "Shape.h"
#include <PxShape.h>
#include <PxFiltering.h>

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

	void CShape::SetCollisionLayers(const ECollisionLayer& aLayer)
	{
		physx::PxFilterData filterData;

		filterData.word0 = aLayer;
		physx::PxU32 collideAll = 0;
		collideAll |= ~0;			//dont even ask, if you want to anyway talk to Danne The mad wizzard
		filterData.word1 = collideAll;

		myShape->setQueryFilterData(filterData);
		myShape->setSimulationFilterData(filterData);
	}

	void CShape::SetCollisionLayers(const ECollisionLayer& aLayer, const ECollisionLayer& aLayerToCollideWith)
	{
		physx::PxFilterData filterData;

		filterData.word0 = aLayer;
		filterData.word1 = aLayerToCollideWith;

		myShape->setQueryFilterData(filterData);
		myShape->setSimulationFilterData(filterData);
	}

}
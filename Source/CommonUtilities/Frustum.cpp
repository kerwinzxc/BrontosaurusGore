#include "stdafx.h"
#include "Frustum.h"
#include "vector3.h"
#include "matrix44.h"
#include "AABB.h"
#include "..\Components\ComponentMessage.h"

namespace
{
	float DistancePoints2(const CU::Vector3f aFirstVector, const CU::Vector3f aSecondVector)
	{
		return ((aFirstVector.x - aSecondVector.x) * (aFirstVector.x - aSecondVector.x)) + 
			((aFirstVector.y - aSecondVector.y) * (aFirstVector.y - aSecondVector.y)) + 
			((aFirstVector.z - aSecondVector.z) * (aFirstVector.z - aSecondVector.z));
	}
}




namespace CU
{

	CFrustum::CFrustum()
	{

	}


	CFrustum::~CFrustum()
	{
	}
	
	void CFrustum::SetFrustum(float aFar, float aNear, float aFOV, float aWidth, float aHeight)
	{
		if (myPlanes.Count() > 0)
		{
			myPlanes.RemovePlanes();
		}

		float FovExtens = 0.0f;//15.0f;

		float AspectRatioY = aHeight / aWidth;
		float FovX = (aFOV + FovExtens) * (static_cast<float>(3.14159265f) / 180.0f);
		float TanFovX = std::tan(FovX / 2.0f);
		float FovY = 2.0f * std::atan(TanFovX * AspectRatioY);


		Plane<float> farPlane = Plane<float>(CU::Point3f(0.f, 0.f, aFar), CU::Vector3f(0.f, 0.f, 1.0f));
		Plane<float> nearPlane = Plane<float>(CU::Point3f(0.f, 0.f, aNear), Vector3f(0.f, 0.f, -1.0f));

		Plane<float> leftPlane;
		Plane<float> rightPlane;
		Plane<float> topPlane;
		Plane<float> bottomPlane;

		//Ortho
		if (aFOV == 0.0f)
		{
			leftPlane = Plane<float>(Point3f((aWidth / 2), 0.f, 0.f), Point3f(1.f, 0.f, 0.f));
			rightPlane = Plane<float>(Point3f(-(aWidth / 2), 0.f, 0.f), Point3f(-1.f, 0.f, 0.f));
			topPlane = Plane<float>(Point3f(0.f, (aHeight / 2), 0.f), Point3f(0.f, 1.f, 0.f));
			bottomPlane = Plane<float>(Point3f(0.f, -(aHeight / 2), 0.f), Point3f(0.f, -1.f, 0.f));
		}
		else //projo
		{
			leftPlane = Plane<float>(Point3f(0.f, 0.f, 0.f),
				Point3f(1.f, 0.f, 0.f) * Matrix33f::CreateRotateAroundY(FovX));

			rightPlane = Plane<float>(Point3f(0.f, 0.f, 0.f),
				Point3f(-1.f, 0.f, 0.f) * Matrix33f::CreateRotateAroundY(-FovX));

			topPlane = Plane<float>(Point3f(0.f, 0.f, 0.f),
				Point3f(0.f, 1.f, 0.f) * Matrix33f::CreateRotateAroundX(-FovY));

			bottomPlane = Plane<float>(Point3f(0.f, 0.f, 0.f),
				Point3f(0.f, -1.f, 0.f) * Matrix33f::CreateRotateAroundX(FovY));
		}


		myPlanes.AddPlane(farPlane);
		myPlanes.AddPlane(nearPlane);
		myPlanes.AddPlane(leftPlane);
		myPlanes.AddPlane(rightPlane);
		myPlanes.AddPlane(topPlane);
		myPlanes.AddPlane(bottomPlane);


	}
//
//	void CFrustum::SetFrustum(const Matrix44f& aViewMatrix)
//	{
//		Plane<float> planes[6];
//
//#define m(row, col) matrix[col*4+row - 5]
//
//		Matrix44f matrix = aViewMatrix;
//
//		planes[0].SetWithCoefficients(
//			m(3, 1) + m(4, 1),
//			m(3, 2) + m(4, 2),
//			m(3, 3) + m(4, 3),
//			m(3, 4) + m(4, 4));
//		planes[1].SetWithCoefficients(
//			-m(3, 1) + m(4, 1),
//			-m(3, 2) + m(4, 2),
//			-m(3, 3) + m(4, 3),
//			-m(3, 4) + m(4, 4));
//		planes[2].SetWithCoefficients(
//			m(2, 1) + m(4, 1),
//			m(2, 2) + m(4, 2),
//			m(2, 3) + m(4, 3),
//			m(2, 4) + m(4, 4));
//		planes[3].SetWithCoefficients(
//			-m(2, 1) + m(4, 1),
//			-m(2, 2) + m(4, 2),
//			-m(2, 3) + m(4, 3),
//			-m(2, 4) + m(4, 4));
//		planes[4].SetWithCoefficients(
//			m(1, 1) + m(4, 1),
//			m(1, 2) + m(4, 2),
//			m(1, 3) + m(4, 3),
//			m(1, 4) + m(4, 4));
//		planes[5].SetWithCoefficients(
//			-m(1, 1) + m(4, 1),
//			-m(1, 2) + m(4, 2),
//			-m(1, 3) + m(4, 3),
//			-m(1, 4) + m(4, 4));
//
//#undef m
//
//	}
//
	bool CFrustum::IsInside(const CU::AABB & aAABB)
	{
	//	// assumes the AABB is in the same space as the Frustum
		bool intersects =
			myPlanes.Inside(aAABB.myCenterPos, aAABB.myRadius);// |
			//myPlanes.Inside({ aAABB.myMinPos.x, aAABB.myMinPos.y, aAABB.myMinPos.z }) |
			//myPlanes.Inside({ aAABB.myMaxPos.x, aAABB.myMinPos.y, aAABB.myMinPos.z }) |
			//myPlanes.Inside({ aAABB.myMinPos.x, aAABB.myMaxPos.y, aAABB.myMinPos.z }) |
			//myPlanes.Inside({ aAABB.myMinPos.x, aAABB.myMinPos.y, aAABB.myMaxPos.z }) |
			//myPlanes.Inside({ aAABB.myMaxPos.x, aAABB.myMaxPos.y, aAABB.myMaxPos.z }) |
			//myPlanes.Inside({ aAABB.myMinPos.x, aAABB.myMaxPos.y, aAABB.myMaxPos.z }) |
			//myPlanes.Inside({ aAABB.myMaxPos.x, aAABB.myMinPos.y, aAABB.myMaxPos.z }) |
			//myPlanes.Inside({ aAABB.myMaxPos.x, aAABB.myMaxPos.y, aAABB.myMinPos.z });


		return intersects;
	}
}
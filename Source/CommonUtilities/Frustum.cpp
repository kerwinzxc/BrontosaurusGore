#include "stdafx.h"
#include "Frustum.h"
#include "vector3.h"
#include "matrix44.h"
#include "Sphere.h"
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

	void CFrustum::SetFrustum(const CU::Matrix44f& aProjectionInverse)
	{
		if (myPlanes.Count() > 0)
		{
			myPlanes.RemovePlanes();
		}


		CU::Vector3f frustumPoints[8] =
		{
			//Far plane
			CU::Vector3f /*topFarLeft		*/(-1.0f, 1.0f, 1.0f),
			CU::Vector3f /*topFarRight		*/(1.0f, 1.0f, 1.0f),
			CU::Vector3f /*bottomFarLeft	*/(-1.0f, -1.0f, 1.0f),
			CU::Vector3f /*bottomFarRight	*/(1.0f, -1.0f, 1.0f),
			//Near Plane
			CU::Vector3f /*topNearLeft		*/(-1.0f, 1.0f, 0.0f),
			CU::Vector3f /*topNearRight		*/(1.0f, 1.0f, 0.0f),
			CU::Vector3f /*bottomNearLeft	*/(-1.0f, -1.0f, 0.0f),
			CU::Vector3f /*bottomNearRight	*/(1.0f, -1.0f, 0.0f)
		};
		
		for (CU::Vector3f& point : frustumPoints)
		{
			CU::Vector4f pointo(point);
			pointo.w = 1.0f;
			pointo = pointo * aProjectionInverse;
			pointo /= pointo.w;
			point = pointo;
		}
		//Plane<float> farPlane(bottomFarLeft, bottomFarRight, topFarLeft);
		//Plane<float> nearPlane(bottomNearRight, bottomNearLeft, topNearRight);
		//Plane<float> leftPlane(bottomNearLeft, bottomFarLeft, topNearLeft);
		//Plane<float> rightPlane(bottomFarRight, bottomNearRight, topFarRight);
		//Plane<float> topPlane(topFarLeft, topFarRight, topNearLeft);
		//Plane<float> bottomPlane(bottomNearLeft, bottomNearRight, bottomFarLeft);

		Plane<float> farPlane(		frustumPoints[2], frustumPoints[3], frustumPoints[0]);
		Plane<float> nearPlane(		frustumPoints[7], frustumPoints[6], frustumPoints[5]);
		Plane<float> leftPlane(		frustumPoints[6], frustumPoints[2], frustumPoints[4]);
		Plane<float> rightPlane(	frustumPoints[3], frustumPoints[7], frustumPoints[1]);
		Plane<float> topPlane(		frustumPoints[0], frustumPoints[1], frustumPoints[4]);
		Plane<float> bottomPlane(	frustumPoints[6], frustumPoints[7], frustumPoints[2]);

		myPlanes.AddPlane(farPlane);
		myPlanes.AddPlane(nearPlane);
		myPlanes.AddPlane(leftPlane);
		myPlanes.AddPlane(rightPlane);
		myPlanes.AddPlane(topPlane);
		myPlanes.AddPlane(bottomPlane);
	}

	bool CFrustum::IsInside(const CU::Sphere& aSphere)
	{
		// assumes the AABB is in the same space as the Frustum
		bool intersects = myPlanes.Inside(aSphere.myCenterPos, aSphere.myRadius);
		return intersects;
	}
}
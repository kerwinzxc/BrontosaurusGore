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

	bool CFrustum::IsInside(const CU::Sphere& aSphere)
	{
		// assumes the AABB is in the same space as the Frustum
		bool intersects =
myPlanes.Inside(aSphere.myCenterPos, aSphere.myRadius);
		return intersects;
	}
}
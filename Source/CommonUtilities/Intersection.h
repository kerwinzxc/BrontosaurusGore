#pragma once
#include "vector2.h"
#include "vector3.h"
#include "matrix.h"
#include "plane.h"
#include "line.h"
#include <math.h>

namespace CU
{
	namespace Intersection
	{

		struct Sphere
		{
			CU::Vector3f myCenterPosition;
			float myRadius;
			float myRadiusSquared;

		};

		struct AABB
		{
			CU::Vector3f myCenterPos;
			CU::Vector3f myExtents;
			CU::Vector3f myMinPos;
			CU::Vector3f myMaxPos;
		};

		struct LineSegment3D
		{
			CU::Vector3f myStartPos;
			CU::Vector3f myEndPos;
		};

		struct LineSegment2D
		{
			CU::Vector2f myStartPos;
			CU::Vector2f myEndPos;
		};

		struct Fov90Frustum
		{
			Fov90Frustum(float aNearZ, float aFarZ);

			CU::Plane<float> myFarPlane;
			CU::Plane<float> myNearPlane;
			CU::Plane<float> myTopPlane;
			CU::Plane<float> myBottomPlane;
			CU::Plane<float> myRightPlane;
			CU::Plane<float> myLeftPlane;

		};


		CU::Vector3f ClosestPointOnAABB(CU::Vector3f aPoint, const AABB &aAABB);
		float Distance2Points(const CU::Vector3f aFirstVector, const CU::Vector3f aSecondVector);

		bool PointInsideSphere(const Sphere &aSpehere, const CU::Vector3f &aPoint);
		bool PointInsideAABB(const AABB &aAABB, const CU::Vector3f &aPoint);

		bool LineVsLine(LineSegment2D aLineSegment, LineSegment2D aSecondLineSegment, CU::Vector3f aIntersectionPoint);
		bool LineVsLine(LineSegment3D aLineSegment, LineSegment3D aSecondLineSegment, CU::Vector3f aIntersectionPoint);

		bool LineVsAABB(const LineSegment3D aLineSegment, const AABB aAABB, const CU::Vector3f aInsersection);
		bool LineVsSphere(const LineSegment3D aLineSegment, const Sphere aSphere, const CU::Vector3f &aIntersectionPoint);

		bool SweptSphereVsAABB(const LineSegment3D aLine, const float aRadius, const AABB aAABB);
		bool SweptSphereVsSphere(const LineSegment3D aLine, const float aRadius, const Sphere aSphere);

		bool SphereVsSphere(const Sphere aFirst, const Sphere aSecond);

		bool SphereVsPlane(CU::Plane<float> aPlane, Sphere aSphere);
		bool SphereVsFrustum(const Sphere aSphere, const Fov90Frustum aFrustum);
	}
}
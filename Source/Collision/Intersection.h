#pragma once

//move all the structs to files of their own and clear out includes from this header file
#include <CommonUtilities.h> // Byt till Utillities.  // PSST Cleara ut collisions från CU
#include <Vector3.h>
#include <vector2.h>
#include "plane.h"

namespace Collision
{
	template<typename TYPE>
	class Line;

	template<typename TYPE>
	class Plane;
}

namespace Intersection
{
	struct SSphere
	{
		CU::Vector3f myCenterPosition;
		float myRadius;
	};

	struct SCircle
	{
		CU::Vector2f myCenterPosition;
		float myRadius;
	};

	struct SPoint
	{
		SPoint() {}
		SPoint(const CU::Vector2f aPosition) : myPosition(aPosition) {}
		CU::Vector2f myPosition;
	};

	struct SSquare 
	{
		CU::Vector2f myMinPosition;
		CU::Vector2f myMaxPosition;
	};

	struct STriangle 
	{
		CU::StaticArray<CU::Vector2f, 3> myPoints;
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

		Collision::Plane<float> myFarPlane;
		Collision::Plane<float> myNearPlane;
		Collision::Plane<float> myTopPlane;
		Collision::Plane<float> myBottomPlane;
		Collision::Plane<float> myRightPlane;
		Collision::Plane<float> myLeftPlane;

	};
	
	union CollisionData
	{
		SSphere* mySphereData;
		SCircle* myCircleData;
		SPoint* myPointData;
		SSquare* mySquareData;
		AABB* myAABBData;
		LineSegment3D* myLineSegment3DData;
		LineSegment2D* myLineSegment2DData;
		Fov90Frustum* myFov90FrustumData;
	};
	
	void ClosestPointOnAABB(CU::Vector3f& aPoint, const AABB &aAABB);
	float Distance2Points(const CU::Vector3f& aFirstVector, const CU::Vector3f& aSecondVector);
	float Distance2Points2(const CU::Vector3f& aFirstVector, const CU::Vector3f& aSecondVector);

	bool PointInsideSphere(const SSphere& aSpehere, const CU::Vector3f& aPoint);
	bool PointInsideAABB(const AABB& aAABB, const CU::Vector3f& aPoint);

	bool LineVsLine(const LineSegment2D& aLineSegment, const LineSegment2D& aSecondLineSegment, CU::Vector3f* aIntersectionPoint = nullptr);
	bool LineVsAABB(const LineSegment3D& aLineSegment, const AABB& aAABB);
	bool LineVsSphere(const LineSegment3D& aLineSegment, const SSphere& aSphere, const CU::Vector3f& aIntersectionPoint);

	bool SweptSphereVsAABB(const LineSegment3D& aLine, const float aRadius, const AABB& aAABB);
	bool SweptSphereVsSphere(const LineSegment3D& aLine, const float aRadius, const SSphere& aSphere);
	bool SphereVsSphere(const SSphere& aFirst, const SSphere& aSecond);

	bool SphereVsPlane(const Collision::Plane<float>& aPlane, const SSphere& aSphere);
	bool SphereVsFrustum(const SSphere& aSphere, const Fov90Frustum& aFrustum);

	bool CircleVsCircle(const SCircle& aFirst, const SCircle& aSecond);
	bool PointInsideCircle(const SCircle& aCircle, const SPoint& aPoint);
	bool PointInsideSquare(const SSquare& aSquare, const SPoint& aPoint);
	bool PointInsideTriangle(const STriangle& aTriangle, const SPoint& aPoint);
	bool CircleVsSquare(const SCircle& aCircle, const SSquare& aSquare);
	bool CircleVsTriangle(const SCircle& aCircle, const STriangle& aTriangle);
	bool SquareVsTriangle(const SSquare& aSquare, const STriangle& aTriangle);
	bool SquareVsSquare(const SSquare& aFirst, const SSquare& aSecond);
}
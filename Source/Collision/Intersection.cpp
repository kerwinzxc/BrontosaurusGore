#include "stdafx.h"
#include "Intersection.h"

#include "matrix.h"
#include "line.h"

#include <cmath>

using namespace Collision;
using namespace CU;

Intersection::Fov90Frustum::Fov90Frustum(float aNearZ, float aFarZ)
{
	myFarPlane = Plane<float>(Point3f(0, 0, -aFarZ), Vector3f(0, 0, -1));
	myNearPlane = Plane<float>(Point3f(0, 0, -aNearZ), Vector3f(0, 0, 1));
	myLeftPlane = Plane<float>(Point3f(0, 0, 0),
		Point3f(1, 0, 0) * Matrix33f::CreateRotateAroundY(PI / 4.0f + PI));

	myRightPlane = Plane<float>(Point3f(0, 0, 0),
		Point3f(1, 0, 0) * Matrix33f::CreateRotateAroundY(-PI / 4.0f));

	myTopPlane = Plane<float>(Point3f(0, 0, 0),
		Point3f(0, 1, 0) * Matrix33f::CreateRotateAroundX(-PI / 4.0f + PI));

	myBottomPlane = Plane<float>(Point3f(0, 0, 0),
		Point3f(0, 1, 0) * Matrix33f::CreateRotateAroundX(PI / 4.0f));
}


void Intersection::ClosestPointOnAABB(Vector3f& aPoint, const AABB& aAABB)
{
	if (aPoint.x < aAABB.myMinPos.x)
	{
		aPoint.x = aAABB.myMinPos.x;
	}
	else if (aPoint.x > aAABB.myMaxPos.x)
	{
		aPoint.x = aAABB.myMaxPos.x;
	}

	if (aPoint.y < aAABB.myMinPos.y)
	{
		aPoint.y = aAABB.myMinPos.y;
	}
	else if (aPoint.y > aAABB.myMaxPos.y)
	{
		aPoint.y = aAABB.myMaxPos.y;
	}

	if (aPoint.z < aAABB.myMinPos.z)
	{
		aPoint.z = aAABB.myMinPos.z;
	}
	else if (aPoint.z > aAABB.myMaxPos.z)
	{
		aPoint.z = aAABB.myMaxPos.z;
	}
}

float Intersection::Distance2Points(const Vector3f& aFirstVector, const Vector3f& aSecondVector)
{
	return sqrt(SQUARE(aFirstVector.x - aSecondVector.x) + SQUARE(aFirstVector.y - aSecondVector.y) + SQUARE(aFirstVector.z - aSecondVector.z));
}

float Intersection::Distance2Points2(const Vector3f& aFirstVector, const Vector3f& aSecondVector)
{
	return SQUARE(aFirstVector.x - aSecondVector.x) + SQUARE(aFirstVector.y - aSecondVector.y) + SQUARE(aFirstVector.z - aSecondVector.z);
}

bool Intersection::PointInsideSphere(const SSphere& aSpehere, const Vector3f& aPoint)
{
	float distance = Distance2Points(aPoint, aSpehere.myCenterPosition);

	if (distance < aSpehere.myRadius)
	{
		return true;
	}
	return false;
}

bool Intersection::PointInsideAABB(const AABB& aAABB, const Vector3f& aPoint)
{
	if (aPoint.x > aAABB.myMinPos.x && aPoint.x < aAABB.myMaxPos.x &&
		aPoint.y > aAABB.myMinPos.y && aPoint.y < aAABB.myMaxPos.y &&
		aPoint.z > aAABB.myMinPos.z && aPoint.z < aAABB.myMaxPos.z)
	{
		return true;
	}

	return false;
}

bool Intersection::LineVsLine(const LineSegment2D& aLineSegment, const LineSegment2D& aSecondLineSegment, Vector3f* aIntersectionPoint)
{
	Vector2f firstLine = aLineSegment.myEndPos - aLineSegment.myStartPos;
	Vector2f secondLine = aSecondLineSegment.myEndPos - aSecondLineSegment.myStartPos;


	//01:09 PM 1/26/2016
	//After hours of speculation, my crew of scientists discovered that the firstXsecond variable is the angular 
	//difference between the two line segments
	//this will revolutionise the rest of this algorithm
	float firstXsecond = firstLine.Cross(secondLine);

	//-||- At the begining of the lines. 
	//the distace
	float startsXfirst = (aSecondLineSegment.myStartPos - aLineSegment.myStartPos).Cross(firstLine);


	// Checks wether the lines are collinear (if the line segments lies on the same line) 
	if (firstXsecond == 0 && startsXfirst == 0)
	{

		//checks if the two line segments on the line are overlaping
		if ((0 <= (aSecondLineSegment.myStartPos - aLineSegment.myStartPos).Dot(firstLine)
			&& (aSecondLineSegment.myStartPos - aLineSegment.myStartPos).Dot(firstLine) <= firstLine.Dot(firstLine))
			||
			(0 <= (aLineSegment.myStartPos - aSecondLineSegment.myStartPos).Dot(secondLine)
			&& (aLineSegment.myStartPos - aSecondLineSegment.myStartPos).Dot(secondLine) <= secondLine.Dot(secondLine)))
		{
			return true;
		}
		return false;
	}


	//if angular difference is zero, and the distance between the two line segments are non zero,
	//then the line segments are parallell and therefore never intersecting.
	if (firstXsecond == 0 && !(startsXfirst == 0))
	{
		return false;
	}

	float t = (aSecondLineSegment.myStartPos - aLineSegment.myStartPos).Cross(secondLine) / firstXsecond;
	float u = (aSecondLineSegment.myStartPos - aLineSegment.myStartPos).Cross(firstLine) / firstXsecond;

	if (aIntersectionPoint != nullptr && /*!(FLOAT_IS_ZERO(firstXsecond)) && */(0 <= t && t <= 1) && (0 <= u && u <= 1))
	{
		aIntersectionPoint->x = aLineSegment.myStartPos.x + t * firstLine.x;
		aIntersectionPoint->y = aLineSegment.myStartPos.y + t * firstLine.y;
		return true;
	}

	return false;
}

bool Intersection::LineVsAABB(const LineSegment3D& aLineSegment, const AABB& aAABB)
{
	Point3f nearestOnAABB = aLineSegment.myStartPos;
	ClosestPointOnAABB(nearestOnAABB, aAABB);
	Vector3f startToAABB = nearestOnAABB - aLineSegment.myStartPos;
	Vector3f lineVector = aLineSegment.myEndPos - aLineSegment.myStartPos;
	Vector3f projection = lineVector.GetNormalized() * (lineVector.GetNormalized().Dot(startToAABB));
	Point3f nearestOnLine = projection - aLineSegment.myStartPos;

	return PointInsideAABB(aAABB, nearestOnLine);
}

bool Intersection::LineVsSphere(const LineSegment3D& aLineSegment, const SSphere& aSphere, const Vector3f &aIntersectionPoint)
{
	//   �-(-�-)-� WHAT IF?
	// this senario is if the line crosses the spheres origo with the line segment start and end pos outside the spheres radius
	// Unknown behaviour supermassive error
	aIntersectionPoint;
	if (Distance2Points(aLineSegment.myStartPos, aSphere.myCenterPosition) < aSphere.myRadius ||
		Distance2Points(aLineSegment.myEndPos, aSphere.myCenterPosition) < aSphere.myRadius)
	{
		return true;
	}

	Vector3f d = aLineSegment.myEndPos - aLineSegment.myStartPos;
	Vector3f e = aSphere.myCenterPosition - aLineSegment.myStartPos;

	d.Normalize();
	float a = e.Dot(d);
	float e2 = e.Length2();

	float b2 = e2 - (a * a);

	if (sqrt(b2) < aSphere.myRadius)
	{
		return true;
	}
	return false;
}

bool Intersection::SweptSphereVsAABB(const LineSegment3D& aLine, const float aRadius, const AABB& aAABB)
{
	if (aRadius == 0.0f)
	{
		return LineVsAABB(aLine, aAABB);
	}


	Point3f nearestOnAABB = aLine.myStartPos;
	ClosestPointOnAABB(nearestOnAABB, aAABB);
	Vector3f startToAABB = nearestOnAABB - aLine.myStartPos;
	Vector3f lineVector = aLine.myEndPos - aLine.myStartPos;

	//the length of the sweptsphere scaled 0 to 1
	float length = startToAABB.Dot(lineVector);
	length /= lineVector.Length2();

	float shortestDistance;

	if (length < 0.0f)
	{
		shortestDistance = Distance2Points(nearestOnAABB, aLine.myStartPos);
	}
	else if (length > 1.0f)
	{
		shortestDistance = Distance2Points(nearestOnAABB, aLine.myEndPos);
	}
	else
	{
		Vector3f projection = aLine.myStartPos + ((lineVector)* length);
		shortestDistance = Distance2Points(nearestOnAABB, projection);
	}
	if (shortestDistance - aRadius < 0.0f)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool Intersection::SweptSphereVsSphere(const LineSegment3D& aLine, const float aRadius, const SSphere& aSphere)
{
	SSphere tempSphere;
	tempSphere.myCenterPosition = aSphere.myCenterPosition;
	tempSphere.myRadius = aSphere.myRadius + aRadius;

	return LineVsSphere(aLine, tempSphere, Vector3f::Zero);
}

bool Intersection::SphereVsSphere(const SSphere& aFirst, const SSphere& aSecond)
{
	float distance = Distance2Points2(aFirst.myCenterPosition, aSecond.myCenterPosition);
	float Radia2 = (aFirst.myRadius + aSecond.myRadius) * (aFirst.myRadius + aSecond.myRadius);
	if (distance < Radia2)
	{
		return true;
	}
	return false;
}

bool Intersection::SphereVsPlane(const Plane<float>& aPlane, const SSphere& aSphere)
{
	CU::Vector3f negatedSphereCenter = -aSphere.myCenterPosition;

	float planeDot = aPlane.myPoint.Dot(aPlane.myNormal);
	float distance = aPlane.myNormal.Dot(negatedSphereCenter) - planeDot;

	//the sphere is completly outside plane
	if (distance > aSphere.myRadius)
	{
		return false;
	}

	//the sphere is completly inside plane
	if (distance < -aSphere.myRadius)
	{
		return true;
	}

	//The sphere intersects with le plan�
	return true;
}

bool Intersection::SphereVsFrustum(const SSphere& aSphere, const Fov90Frustum& aFrustum)
{

	//	 ~~~~~=?~~~~~~~~~~~~~~~~
	//	  _____|_______  
	//	>(0)_v______0__)-x ~~~~     <- A ASCII-art scetch of our vessle.
	//									The Sub: S.S Stroustrup.
	//
	//As our journey continue we encounter a new type of collision checking,
	// in order to pass this test of greatness we must find a new way to check 
	// if our sphere is intersecting with a plane of infinite size. 
	// We must do this test in six steps, with six planes that shape the most 
	// horrible shapes of them all......The Frusum.

	if ((SphereVsPlane(aFrustum.myBottomPlane, aSphere) == true) &&
		(SphereVsPlane(aFrustum.myTopPlane, aSphere) == true) &&
		(SphereVsPlane(aFrustum.myLeftPlane, aSphere) == true) &&
		(SphereVsPlane(aFrustum.myRightPlane, aSphere) == true) &&
		(SphereVsPlane(aFrustum.myNearPlane, aSphere) == true) &&
		(SphereVsPlane(aFrustum.myFarPlane, aSphere) == true))
	{
		return true;
	}

	return false;
}

bool Intersection::CircleVsCircle(const SCircle& aFirst, const SCircle& aSecond)
{
	float distance2 = (aFirst.myCenterPosition - aSecond.myCenterPosition).Length2();
	float radii2 = (aFirst.myRadius + aSecond.myRadius) * (aFirst.myRadius + aSecond.myRadius);

	return distance2 <= radii2;
}

bool Intersection::PointInsideCircle(const SCircle& aCircle, const SPoint& aPoint)
{
	float distance2 = (aPoint.myPosition - aCircle.myCenterPosition).Length2();
	return distance2 <= aCircle.myRadius * aCircle.myRadius;
}

bool Intersection::PointInsideSquare(const SSquare& aSquare, const SPoint& aPoint)
{
	if (aPoint.myPosition.x < aSquare.myMinPosition.x)
	{
		return false;
	}
	else if (aPoint.myPosition.y < aSquare.myMinPosition.y)
	{
		return false;
	}
	else if (aPoint.myPosition.x >= aSquare.myMaxPosition.x)
	{
		return false;
	}
	else if (aPoint.myPosition.y >= aSquare.myMaxPosition.y)
	{
		return false;
	}

	return true;
}

bool Intersection::PointInsideTriangle(const STriangle& aTriangle, const SPoint& aPoint)
{
	//if point inside all lines of triangle: true


	return false;
}

bool Intersection::CircleVsSquare(const SCircle& aCircle, const SSquare& aSquare)
{
	SSquare extendedSquare;
	extendedSquare.myMaxPosition = aSquare.myMaxPosition + CU::Vector2f::One * aCircle.myRadius;
	extendedSquare.myMinPosition = aSquare.myMinPosition - CU::Vector2f::One * aCircle.myRadius;
	
	return PointInsideSquare(extendedSquare, SPoint(aCircle.myCenterPosition));
}

bool Intersection::CircleVsTriangle(const SCircle& aCircle, const STriangle& aTriangle)
{
	assert(!"not implemented");
	//don't know, alt: circle collides with either line of triangle || circle is completely inside triangle || triangle is completely inside circle
	return false;
}

bool Intersection::SquareVsTriangle(const SSquare& aSquare, const STriangle& aTriangle)
{
	assert(!"not implemented");
	//don't know, alt: circle collides with either line of triangle || circle is completely inside triangle || triangle is completely inside circle
	return false;
}

bool Intersection::SquareVsSquare(const SSquare& aFirst, const SSquare& aSecond)
{
	if (aFirst.myMaxPosition.x < aSecond.myMinPosition.x)
	{
		return false;
	}

	if (aFirst.myMaxPosition.y < aSecond.myMinPosition.y)
	{
		return false;
	}

	if (aFirst.myMinPosition.x > aSecond.myMaxPosition.x)
	{
		return false;
	}

	if (aFirst.myMinPosition.y > aSecond.myMaxPosition.y)
	{
		return false;
	}

	return true;
}

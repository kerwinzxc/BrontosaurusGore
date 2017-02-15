#include "stdafx.h"
#include "Intersection.h"
#include "CommonUtilities.h"

namespace CU
{

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


	Vector3f Intersection::ClosestPointOnAABB(Vector3f aPoint, const AABB &aAABB)
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

		return aPoint;
	}

	float Intersection::Distance2Points(const Vector3f aFirstVector, const Vector3f aSecondVector)
	{
		return sqrt(SQUARE(aFirstVector.x - aSecondVector.x) + SQUARE(aFirstVector.y - aSecondVector.y) + SQUARE(aFirstVector.z - aSecondVector.z));
	}


	bool Intersection::PointInsideSphere(const Sphere &aSpehere, const Vector3f &aPoint)
	{
		float distance = Distance2Points(aPoint, aSpehere.myCenterPosition);

		if (distance < aSpehere.myRadius)
		{
			return true;
		}
		return false;
	}

	bool Intersection::PointInsideAABB(const AABB &aAABB, const Vector3f &aPoint)
	{
		if (aPoint.x > aAABB.myMinPos.x && aPoint.x < aAABB.myMaxPos.x &&
			aPoint.y > aAABB.myMinPos.y && aPoint.y < aAABB.myMaxPos.y &&
			aPoint.z > aAABB.myMinPos.z && aPoint.z < aAABB.myMaxPos.z)
		{
			return true;
		}

		return false;
	}

	bool Intersection::LineVsLine(LineSegment2D aLineSegment, LineSegment2D aSecondLineSegment, Vector3f aIntersectionPoint)
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

		if (/*!(FLOAT_IS_ZERO(firstXsecond)) && */(0 <= t && t <= 1) && (0 <= u && u <= 1))
		{
			aIntersectionPoint.x = aLineSegment.myStartPos.x + t * firstLine.x;
			aIntersectionPoint.y = aLineSegment.myStartPos.y + t * firstLine.y;
			return true;
		}

		return false;
	}

	bool Intersection::LineVsAABB(const LineSegment3D aLineSegment, const AABB aAABB, const Vector3f aIntersection)
	{
		aIntersection;
		Point3f nearestOnAABB = ClosestPointOnAABB(aLineSegment.myStartPos, aAABB);
		Vector3f startToAABB = nearestOnAABB - aLineSegment.myStartPos;
		Vector3f lineVector = aLineSegment.myEndPos - aLineSegment.myStartPos;
		Vector3f projection = lineVector.GetNormalized() * (lineVector.GetNormalized().Dot(startToAABB));
		Point3f nearestOnLine = projection - aLineSegment.myStartPos;

		return PointInsideAABB(aAABB, nearestOnLine);
	}

	bool Intersection::LineVsSphere(const LineSegment3D aLineSegment, const Sphere aSphere, const Vector3f &aIntersectionPoint)
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

	bool Intersection::SweptSphereVsAABB(const LineSegment3D aLine, const float aRadius, AABB aAABB)
	{

		if (aRadius == 0.0f)
		{
			return LineVsAABB(aLine, aAABB, Vector3f::Zero);
		}


		Point3f nearestOnAABB = ClosestPointOnAABB(aLine.myStartPos, aAABB);
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

	bool Intersection::SweptSphereVsSphere(const LineSegment3D aLine, const float aRadius, const Sphere aSphere)
	{
		Sphere tempSphere;
		tempSphere.myCenterPosition = aSphere.myCenterPosition;
		tempSphere.myRadius = aSphere.myRadius + aRadius;
		tempSphere.myRadiusSquared = SQUARE(tempSphere.myRadius);

		return LineVsSphere(aLine, tempSphere, Vector3f::Zero);
	}

	bool Intersection::SphereVsSphere(const Sphere aFirst, const Sphere aSecond)
	{
		if (Distance2Points(aFirst.myCenterPosition, aSecond.myCenterPosition) < (aFirst.myRadius + aSecond.myRadius))
		{
			return true;
		}
		return false;
	}

	bool Intersection::SphereVsPlane(Plane<float> aPlane, Sphere aSphere)
	{
		aSphere.myCenterPosition.x = -aSphere.myCenterPosition.x;
		aSphere.myCenterPosition.y = -aSphere.myCenterPosition.y;
		aSphere.myCenterPosition.z = -aSphere.myCenterPosition.z;

		float planeDot = aPlane.myPoint.Dot(aPlane.myNormal);
		float distance = aPlane.myNormal.Dot(aSphere.myCenterPosition) - planeDot;

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

	bool Intersection::SphereVsFrustum(const Sphere aSphere, const Fov90Frustum aFrustum)
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

}
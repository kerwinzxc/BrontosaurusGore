#pragma once
#include "vector3.h"
#define LINE3D_TEMPLATE template<typename TYPE>

namespace CU
{
	template<typename TYPE>
	class Line3D
	{
	public:     // Sets line to (0,0) -> (0,0) as this will produce most errors making sure the
				// user notices that the line isn't initialized properly.
		Line3D(void);

		// Initializes the line from two points
		//
		Line3D(const Point3<TYPE> &aFirstPoint, const Point3<TYPE> &aSecondPoint);

		// Sets the line data using two points
		//
		void SetWith2Points(const Point3<TYPE> &aFirstPoint, const Point3<TYPE> &aSecondPoint);

		// Sets the line data using a point and a direction
		//
		void SetWithPointAndDirection(const Point3<TYPE> &aPoint, const Vector3<TYPE> &aDirection);

		// Returns true if the point is on the inside of the line. The inside is defined as the
		// side opposite the normal. That is, the normal points to the outside of the line.
		//
		bool IsInside(const Point3<TYPE> &aPoint) const;

		// Checks two lines for equality. Returns true if aLeft is the same line as aRight.
		//
		friend bool operator ==(const Line3D<TYPE> &aLeft, const Line3D<TYPE> &aRight)
		{
			if (aLeft.myFirstPoint == aRight.myFirstPoint && 
				aLeft.myDirection == aRight.myDirection)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		// Checks two lines for inequality. Returns true if aLeft is not the same line as aRight.
		//
		friend bool operator !=(const Line3D<TYPE> &aLeft, const Line3D<TYPE> &aRight)
		{
			return (!(aLeft == aRight));
		}

		Point3<TYPE> myFirstPoint;
		Vector3<TYPE> myDirection;
		Vector3<TYPE> myNormal;

	private:    // Line representation goes here

	};

	LINE3D_TEMPLATE
		Line3D<TYPE>::Line3D()
	{
		myFirstPoint.Zero;
		myDirection.Zero;

		myNormal.Zero;
	}

	LINE3D_TEMPLATE
		Line3D<TYPE>::Line3D(const Point3<TYPE> &aFirstPoint, const Point3<TYPE> &aSecondPoint)
	{
		myFirstPoint = aFirstPoint;
		myDirection = aSecondPoint - aFirstPoint;

		myNormal.x = myDirection.y;
		myNormal.y = -myDirection.x;
	}

	LINE3D_TEMPLATE
		void Line3D<TYPE>::SetWith2Points(const Point3<TYPE> &aFirstPoint, const Point3<TYPE> &aSecondPoint)
	{
		myFirstPoint = aFirstPoint;
		myDirection = aSecondPoint - aFirstPoint;

		myNormal.x = myDirection.y;
		myNormal.y = -myDirection.x;
	}

	LINE3D_TEMPLATE
		void Line3D<TYPE>::SetWithPointAndDirection(const Point3<TYPE> &aPoint, const Vector3<TYPE> &aDirection)
	{
		myFirstPoint = aPoint;
		myDirection = aDirection;

		myNormal.x = myDirection.y;
		myNormal.y = -myDirection.x;
	}

	LINE3D_TEMPLATE
		bool Line3D<TYPE>::IsInside(const Point3<TYPE> &aPoint) const
	{
		if (myNormal.Dot(aPoint - myFirstPoint) < 0)
		{
			return true;
		}
		else
		{
			return false;
		}
	}
} 

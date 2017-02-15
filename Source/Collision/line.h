#pragma once
#include <vector2.h>

#define LINE_TEMPLATE template<typename TYPE>

namespace Collision
{
	template<typename TYPE>
	class Line
	{
	public:     // Sets line to (0,0) -> (0,0) as this will produce most errors making sure the
		// user notices that the line isn't initialized properly.
		Line(void);

		// Initializes the line from two points
		//
		Line(const Point2<TYPE> &aFirstPoint, const Point2<TYPE> &aSecondPoint);

		// Sets the line data using two points
		//
		void SetWith2Points(const Point2<TYPE> &aFirstPoint, const Point2<TYPE> &aSecondPoint);

		// Sets the line data using a point and a direction
		//
		void SetWithPointAndDirection(const Point2<TYPE> &aPoint, const Vector2<TYPE> &aDirection);

		// Returns true if the point is on the inside of the line. The inside is defined as the
		// side opposite the normal. That is, the normal points to the outside of the line.
		//
		bool IsInside(const Point2<TYPE> &aPoint) const;

		// Checks two lines for equality. Returns true if aLeft is the same line as aRight.
		//
		friend bool operator ==(const Line<TYPE> &aLeft, const Line<TYPE> &aRight)
		{
			if (aLeft.myFirstPoint == aRight.myFirstPoint && aLeft.myDirection == aRight.myDirection)
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
		friend bool operator !=(const Line<TYPE> &aLeft, const Line<TYPE> &aRight)
		{
			return (!(aLeft == aRight));
		}

		Point2<TYPE> myFirstPoint;
		Vector2<TYPE> myDirection;
		Vector2<TYPE> myNormal;

	private:    // Line representation goes here

	};

	LINE_TEMPLATE
		Line<TYPE>::Line()
	{
		myFirstPoint.Zero;
		myDirection.Zero;

		myNormal.Zero;
	}

	LINE_TEMPLATE
		Line<TYPE>::Line(const Point2<TYPE> &aFirstPoint, const Point2<TYPE> &aSecondPoint)
	{
		myFirstPoint = aFirstPoint;
		myDirection = aSecondPoint - aFirstPoint;

		myNormal.x = myDirection.y;
		myNormal.y = -myDirection.x;
	}

	LINE_TEMPLATE
		void Line<TYPE>::SetWith2Points(const Point2<TYPE> &aFirstPoint, const Point2<TYPE> &aSecondPoint)
	{
		myFirstPoint = aFirstPoint;
		myDirection = aSecondPoint - aFirstPoint;

		myNormal.x = myDirection.y;
		myNormal.y = -myDirection.x;
	}

	LINE_TEMPLATE
		void Line<TYPE>::SetWithPointAndDirection(const Point2<TYPE> &aPoint, const Vector2<TYPE> &aDirection)
	{
		myFirstPoint = aPoint;
		myDirection = aDirection;

		myNormal.x = myDirection.y;
		myNormal.y = -myDirection.x;
	}

	LINE_TEMPLATE
		bool Line<TYPE>::IsInside(const Point2<TYPE> &aPoint) const
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
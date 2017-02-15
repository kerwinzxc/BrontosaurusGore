#pragma once
#include <vector3.h>

#define PLANE_TEMPLATE template<typename TYPE>

using namespace CU;

namespace Collision
{
	template<typename TYPE>
	class Plane
	{
	public:     // Sets all plane values to 0 to make it obvious to the user that it's not initialized.
		//
		Plane(void);

		// Initializes the plane using three points
		//
		Plane(const Vector3<TYPE> &aFirstPoint, const Vector3<TYPE> &aSecondPoint, const Vector3<TYPE> &aThirdPoint);

		// Initializes the plane using a point and a normal
		//
		Plane(const Vector3<TYPE> &aPoint, const Vector3<TYPE> &aNormal);

		// Sets the plane from three points
		//
		void SetWith3Points(const Vector3<TYPE> &aFirstPoint, const Vector3<TYPE> &aSecondPoint, const Vector3<TYPE> &aThirdPoint);

		// Sets the plane from a point and a normal
		//
		void SetWithPointAndNormal(const Vector3<TYPE> &aPoint, const Vector3<TYPE> &aNormal);

		// Returns true if the point is on the inside of the plane. The inside is defined as
		// the opposite side of where the normal points to.
		//
		bool IsInside(const Vector3<TYPE> &aPoint) const;

		// Checks two planes for equality. Returns true if aLeft is the same plane as aRight.
		//
		friend bool operator ==(const Plane<TYPE> &aLeft, const Plane<TYPE> &aRight)
		{
			if (aLeft.myPoint == aRight.myPoint && aLeft.myNormal == aRight.myNormal)
			{
				return true;
			}
			else
			{
				return false;
			}
		}

		// Checks two planes for inequality. Returns true if aLeft is not the same plane as aRight.
		//
		friend bool operator !=(const Plane<TYPE> &aLeft, const Plane<TYPE> &aRight)
		{
			return !(aLeft == aRight);
		}

		Vector3<TYPE> myPoint;
		Vector3<TYPE> myNormal;
	private:    // Plane representation goes here
	};

	PLANE_TEMPLATE
		Plane<TYPE>::Plane()
	{
		myPoint.Zero;
		myNormal.Zero;
	}

	PLANE_TEMPLATE
		Plane<TYPE>::Plane(const Vector3<TYPE> &aFirstPoint, const Vector3<TYPE> &aSecondPoint, const Vector3<TYPE> &aThirdPoint)
	{
		SetWith3Points(aFirstPoint, aSecondPoint, aThirdPoint);
	}

	PLANE_TEMPLATE
		Plane<TYPE>::Plane(const Vector3<TYPE> &aPoint, const Vector3<TYPE> &aNormal)
	{
		SetWithPointAndNormal(aPoint, aNormal);
	}

	PLANE_TEMPLATE
		void Plane<TYPE>::SetWith3Points(const Vector3<TYPE> &aFirstPoint, const Vector3<TYPE> &aSecondPoint, const Vector3<TYPE> &aThirdPoint)
	{
		myPoint = aFirstPoint;
		myNormal = Vector3<TYPE>::Cross(aThirdPoint - aFirstPoint, aSecondPoint - aFirstPoint);
	}

	PLANE_TEMPLATE
		void Plane<TYPE>::SetWithPointAndNormal(const Vector3<TYPE> &aPoint, const Vector3<TYPE> &aNormal)
	{
		myPoint = aPoint;
		myNormal = aNormal;
	}

	PLANE_TEMPLATE
		bool Plane<TYPE>::IsInside(const Vector3<TYPE> &aPoint) const
	{
		Vector3<TYPE> temp = aPoint - myPoint;

		if (temp.Dot(myNormal) < 0)
		{
			return true;
		}

		return false;

	}
}

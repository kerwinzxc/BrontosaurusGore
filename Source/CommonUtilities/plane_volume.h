#pragma once
#include "plane.h"

namespace CU
{
	template<typename TYPE>
	class PlaneVolume
	{
	public:    
		PlaneVolume();

		// Initializes the volume with a list of planes.
		//
		PlaneVolume(const CU::GrowingArray<Plane<TYPE>> &somePlanes);

		// Adds a plane to the volume
		//
		void AddPlane(const Plane<TYPE> &aPlane);

		// Removes a plane from the volume.
		//
		void RemovePlanes();

		// Returns true if the point is inside of all planes in the volume.
		//
		bool Inside(const Point3<TYPE> &aPoint) const;

		bool Inside(const Point3<TYPE> &aPoint, const float aRadius) const;


		unsigned int Count() const;
	private:    // Internal representation of the plane goes here
		CU::GrowingArray<Plane<TYPE>> myPlanes;
	};

	PLANE_TEMPLATE
		PlaneVolume<TYPE>::PlaneVolume(const CU::GrowingArray<Plane<TYPE>> &somePlanes)
	{
		myPlanes = somePlanes;
	}

	PLANE_TEMPLATE
	inline PlaneVolume<TYPE>::PlaneVolume()
	{
		myPlanes.Init(6);
	}

	PLANE_TEMPLATE
	void PlaneVolume<TYPE>::AddPlane(const Plane<TYPE> &aPlane)
	{
		myPlanes.Add(aPlane);
	}
	PLANE_TEMPLATE
	void PlaneVolume<TYPE>::RemovePlanes()
	{
		myPlanes.RemoveAll();
	}

	PLANE_TEMPLATE
	bool PlaneVolume<TYPE>::Inside(const Point3<TYPE> &aPoint) const
	{
		for (int i = 0; i < myPlanes.Size(); ++i)
		{
			if (myPlanes[i].IsInside(aPoint) == false)
			{
				return false;
			}
		}
		return true;
	}


	template<typename TYPE>
	inline bool CU::PlaneVolume<TYPE>::Inside(const Point3<TYPE>& aPoint, const float aRadius) const
	{
		for (unsigned int i = 0; i < myPlanes.Size(); ++i)
		{
			if (myPlanes[i].IsInside(aPoint, aRadius) == false)
			{
				return false;
			}
		}
		return true;
	}



	PLANE_TEMPLATE
	unsigned int PlaneVolume<TYPE>::Count() const
	{
		return myPlanes.Size();

	}
}
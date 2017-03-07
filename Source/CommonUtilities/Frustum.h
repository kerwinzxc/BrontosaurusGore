#pragma once
#include "plane_volume.h"

namespace CU
{
	struct Sphere;
}


namespace CU
{

	class CFrustum
	{
	public:
		CFrustum();
		~CFrustum();


		//fov in degrees
		void SetFrustum(float aFar, float aNear, float aFOV, float aWidth, float aHeight);
		bool IsInside(const CU::Sphere& aSphere);
	private:
		CU::PlaneVolume<float> myPlanes;
	};

}

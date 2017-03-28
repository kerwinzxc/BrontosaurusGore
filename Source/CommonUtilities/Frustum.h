#pragma once
#include "plane_volume.h"

namespace CU
{
	template<typename T>
	class Matrix44;
	using Matrix44f = Matrix44<float>;

	struct Sphere;

	class CFrustum
	{
	public:
		CFrustum();
		~CFrustum();

		//fov in degrees
		void SetFrustum(float aFar, float aNear, float aFOV, float aWidth, float aHeight);
		void SetFrustum(const CU::Matrix44f& aProjectionInverse);
		bool IsInside(const CU::Sphere& aSphere);
	private:
		CU::PlaneVolume<float> myPlanes;
	};
}

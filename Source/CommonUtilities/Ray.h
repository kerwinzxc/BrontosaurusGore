#pragma once
#include "vector3.h"

namespace CU
{
	class Ray
	{
	public:
		Ray();
		Ray(const CU::Vector3f& aPosition, const CU::Vector3f& aDirection);
		~Ray();

		const CU::Vector3f& GetStartPosition() const;
		const CU::Vector3f& GetDirection() const;

		void SetDirection(const CU::Vector3f& aDirection);
		void SetPosition(const CU::Vector3f& aPosition);
		void SetPositionAndDirection(const CU::Vector3f& aPosition, const CU::Vector3f& aDirection);
	private:
		CU::Vector3f myPosition;
		CU::Vector3f myDirection;
	};
}


#include "stdafx.h"
#include "Ray.h"

namespace CU
{
	Ray::Ray()
	{

	}

	Ray::Ray(const CU::Vector3f & aPosition, const CU::Vector3f & aDirection)
	{
		myPosition = aPosition;
		myDirection = aDirection;
	}

	Ray::~Ray()
	{
	}

	const CU::Vector3f & Ray::GetStartPosition() const
	{
		return myPosition;
	}

	const CU::Vector3f & Ray::GetDirection() const
	{
		return myDirection;
	}

	void Ray::SetDirection(const CU::Vector3f & aDirection)
	{
		myDirection = aDirection;
	}

	void Ray::SetPosition(const CU::Vector3f & aPosition)
	{
		myPosition = aPosition;
	}

	void Ray::SetPositionAndDirection(const CU::Vector3f & aPosition, const CU::Vector3f & aDirection)
	{
		myPosition = aPosition;
		myDirection = aDirection;
	}
}

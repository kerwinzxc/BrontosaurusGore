#pragma once
#include "vector3.h"
class Voxel
{
public:
	Voxel();
	Voxel(const Vector3f &aPosition, const Vector3f &aColor, const Vector3f &aNormal);
	~Voxel();

	Vector3f myPosition;
	Vector3f myColour;
	Vector3f myNormal;
};


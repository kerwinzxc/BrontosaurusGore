#include "stdafx.h"
#include "Voxel.h"


Voxel::Voxel()
{
	myPosition = { 0, 0, 0 };
	myColour = { 0, 0, 0 };
	myNormal = { 0, 0, 0 };
}

Voxel::Voxel(const Vector3f &aPosition, const Vector3f &aColor, const Vector3f &aNormal)
{
	myPosition = aPosition;
	myColour = aColor;
	myNormal = aNormal;
}


Voxel::~Voxel()
{
}

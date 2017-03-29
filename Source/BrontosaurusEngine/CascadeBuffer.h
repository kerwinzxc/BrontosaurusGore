#pragma once

#define NUM_CASCADES 4

struct SCascade
{
	void InitProjection(
		const float aLeft,
		const float aRight,
		const float aTop,
		const float aBottom,
		const float aFar,
		const float aNear)
	{
		myOrthoProjection = myOrthoProjection.CreateOrthogonalProjectionMatrixLH(aNear, aFar, aRight - aLeft, aTop - aBottom);
	}
	CU::Matrix44f myOrthoProjection;
	CU::Matrix44f myTransformation;
	CU::Vector4f myRect;
};

struct SCascadeBuffer
{
	SCascade myCascades[NUM_CASCADES];
	float myCascadeEnds[NUM_CASCADES + 1];
	float garbage[3];
};

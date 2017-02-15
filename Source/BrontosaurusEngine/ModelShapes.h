#pragma once

enum class eModelShape
{
	eTriangle = 0,
	eQuad,
	eCube,
	eSphere
};

struct SShape
{
	SShape() {};
	SShape(eModelShape aShape, float aSize) { shape = aShape; size = aSize; }

	eModelShape shape;
	float size = 1;
};
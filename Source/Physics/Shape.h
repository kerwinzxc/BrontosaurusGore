#pragma once

namespace physx
{
	class PxShape;
}

class CShape
{
public:
	CShape(physx::PxShape* aShape);
	~CShape();

private:
	physx::PxShape* myShape;

};


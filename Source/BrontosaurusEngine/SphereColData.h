#pragma once
#include "../CommonUtilities/vector3.h"
//#include "../CommonUtilities/GrowingArray.h"

struct SSphereColData
{
	SSphereColData() { children.Init(2); }
	SSphereColData& operator=(SSphereColData&& aTemporary) { pos = aTemporary.pos; radius = aTemporary.radius; children = std::move(aTemporary.children); return *this; }
	SSphereColData& operator=(const SSphereColData& aTemporary) = default;
	CU::Vector3f pos;
	float radius = 0;
	CU::GrowingArray<SSphereColData*> children;
};

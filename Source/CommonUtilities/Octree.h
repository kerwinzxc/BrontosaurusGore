#pragma once
#include "vector3.h"
#include "Intersection.h"
#include "GrowingArray.h"

class CTreeNode;

//This class shuld be a interface only
class CTreeDweller
{
	CU::Vector3f position, dimensions;
	CTreeNode* node;
};

class CTreeNode
{
public:
	CTreeNode();
	CU::Vector3f center;
	float halfWidth, looseness;
	CTreeNode* children[8];
	CTreeNode* parent;
	CU::GrowingArray<CTreeDweller*> objects;
	CTreeNode* Insert(CTreeDweller* aObject);
};


class COctree
{
public:
	COctree();
	~COctree();

	void Insert(CTreeDweller* aObject);
	CTreeNode * BuildTree(CU::Vector3f aCenter, float aHalfWidth, int aStopDepth, CTreeNode * aParent);
	CU::Intersection::AABB NodeToAABB(CTreeNode* aNode);

private:
	CTreeNode* myRoot;
};


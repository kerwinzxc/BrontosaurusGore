#include "stdafx.h"
#include "Octree.h"


COctree::COctree()
{
	myRoot = nullptr;
}


COctree::~COctree()
{
}

void COctree::Insert(CTreeDweller * /*aObject*/)
{
	
}

CTreeNode * COctree::BuildTree(CU::Vector3f aCenter, float aHalfWidth, int aStopDepth, CTreeNode* aParent = nullptr)
{
	if (aStopDepth < 0)
	{
		return nullptr;
	}
	
	CTreeNode * newNode = new CTreeNode();
	newNode->center = aCenter;
	newNode->halfWidth = aHalfWidth;
	newNode->parent = aParent;
	
	CU::Vector3f offset;
	float step = aHalfWidth * 0.5f;

	//8 is the child size, octree u know
	for (unsigned int childIndex = 0; childIndex < 8; ++childIndex)
	{
		offset.x = childIndex & 1 ? step : -step;
		offset.y = childIndex & 2 ? step : -step;
		newNode->children[childIndex] = BuildTree(offset, step, aStopDepth - 1, newNode);
	}

	return newNode;
}

CU::Intersection::AABB COctree::NodeToAABB(CTreeNode * /*aNode*/)
{
	return CU::Intersection::AABB();
}

CTreeNode::CTreeNode()
{
	objects.Init(8);
}

CTreeNode * CTreeNode::Insert(CTreeDweller * /*aObject*/)
{
	//bool straddle = false;
	//unsigned int childIndex = 0;
	//
	//for (unsigned int componentIndex = 0; componentIndex < 2; componentIndex++)
	//{
	//	//float delta = aObject->position
	//}


	return nullptr;
}

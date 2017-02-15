#pragma once
#include "matrix.h"
#include "Camera.h"
#include "CommonUtilities.h"

#include "GrowingArray.h"
#include "tga2d/sprite/sprite.h"
#include "Voxel.h"
#include "Light.h"
#include <vector>

//namespace DX2D
//{
//	class CSprite;
//	class CColor;
//}

struct RenderObject
{
	Vector3f myPos;
	Vector3f myCol;
	float myScale;

	RenderObject(const Vector3f &aPos, const Vector3f &aCol, const float &aScale)
	{
		myCol = aCol;
		myPos = aPos;
		myScale = aScale;
	}

};

class PointCloud
{
public:
	PointCloud();
	~PointCloud();
	// Adds a new PointCloud in an array of PointCloud pointers. When render 
	// is called the PointCloud will call render on each of it's child clouds. 
	void AddChild(PointCloud* aCloud);
	void AddPoint(Vector3f aPoint, DX2D::CColor aColor);
	void AddPoint(const Voxel &aVoxel);

	void SetColor(const DX2D::CColor aColor);
	void Scale(const float aScale);
	void SetPosition(Vector3f aPosition);
	void Rotate(float aAngle);
	inline Vector3f GetPosition()
	{
		return myLocalSpace.GetPosition();
	}
	void Translate(Vector3f aPosition);

	static bool IsInfront(const RenderObject &aVoxel1, const RenderObject& aVoxel2)
	{
		return aVoxel1.myScale < aVoxel2.myScale;
	}

	// Now this function should take a Camera instance. When you calculate 
	// the render position of each point you use ( point * cameraInverse ) * projection. 
	void Render(const Matrix44f& aParentSpace, const Camera& aCamera, DX2D::CSprite* aSprite, DirectionalLight aLight);

	Matrix44f myLocalSpace;

private:
	CommonUtilities::GrowingArray<PointCloud*, unsigned int> myChildren;
	CommonUtilities::GrowingArray<Vector3f, unsigned int> myPoints;
	std::vector<Voxel> myVoxels;
	std::vector<RenderObject> myRenderObjs;

	CommonUtilities::GrowingArray<DX2D::CColor> myPointColors;

	Matrix44f toWorld;
	Matrix44f myRenderMatrix;
	Matrix44f myOffset;

	Vector3f renderPos;
	DX2D::CColor myColor;
};

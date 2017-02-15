#include "stdafx.h"
#include "PointCloud.h"

PointCloud::PointCloud()
{
	myColor = DX2D::CColor(1.0f, 1.0f, 1.0f, 1.0f);
	myChildren.Init(5);
	myPoints.Init(50);
	myPointColors.Init(50);
	myLocalSpace = Matrix44f::Identity;
	renderPos = Vector3f::Zero;
}

PointCloud::~PointCloud()
{
	//myChildren.DeleteAll();
}

void PointCloud::AddPoint(Vector3f aPoint, DX2D::CColor aColor)
{
	myPoints.Add(aPoint);
	myPointColors.Add(aColor);
}

void PointCloud::AddPoint(const Voxel &aVoxel)
{
	myVoxels.push_back(aVoxel);
}

void PointCloud::AddChild(PointCloud* aCloud)
{
	myChildren.Add(aCloud);
}

void PointCloud::SetColor(const DX2D::CColor aColor)
{
	myColor = aColor;
}

void PointCloud::Scale(const float aScale)
{
	for (unsigned short i = 0; i < myPoints.Size(); ++i)
	{
		myPoints[i].x /= aScale;
		myPoints[i].y /= aScale;
		myPoints[i].z /= aScale;
	}
}

void PointCloud::SetPosition(Vector3f aPosition)
{
	myLocalSpace.SetPosition(aPosition);
}

void PointCloud::Rotate(float aAngle)
{
	myLocalSpace.CreateRotateAroundZ(aAngle);
}

void PointCloud::Translate(Vector3f aPosition)
{
	Matrix44f translationMatrix;
	translationMatrix.SetPosition(aPosition);


	myOffset = translationMatrix;
}

void PointCloud::Render(
	const Matrix44f& aParentSpace,
	const Camera& aCamera,
	DX2D::CSprite* aSprite, DirectionalLight aLight)
{
	toWorld = myLocalSpace * aParentSpace;
	toWorld *= myOffset;
	myRenderMatrix = toWorld * aCamera.GetInverse() * aCamera.GetProjection();

	for (unsigned short i = 0; i < myPoints.Size(); ++i)
	{
		renderPos = myPoints[i] * myRenderMatrix;

		if (renderPos.w < 1.0f || renderPos.w > 1000.0f)
		{
			continue;
		}
		float scale = 1 / renderPos.w;
		renderPos /= renderPos.w;

		Vector3f color = { myPointColors[i].myR, myPointColors[i].myG, myPointColors[i].myB };

		myRenderObjs.push_back(RenderObject(renderPos, color, scale));

	}

	for (unsigned short i = 0; i < myVoxels.size(); ++i)
	{
		//CULTUR STUFFS
		renderPos = myVoxels[i].myPosition * myRenderMatrix;

		Vector3f diffuse, halfVector, specular;
		float specularPower = 16;

		halfVector = Vector3f::Normalize(-aLight.myDirection + (aCamera.GetPosition() - myVoxels[i].myPosition));

		Vector3f preDiff = myVoxels[i].myColour * aLight.myColour * Vector3f::Dot(myVoxels[i].myNormal, -aLight.myDirection);
		diffuse.x = RANGE(preDiff.x, 0.0f, 1.0f);
		diffuse.y = RANGE(preDiff.y, 0.0f, 1.0f);
		diffuse.z = RANGE(preDiff.z, 0.0f, 1.0f);

		float preSpec = Vector3f::Dot(halfVector, myVoxels[i].myNormal);
		preSpec = RANGE(preSpec, 0.0f, 1.0f);

		preSpec = std::pow(preSpec, specularPower);
		specular = preSpec * aLight.myColour;

		Vector3f finalColor = specular + diffuse;


		if (renderPos.w < 1.0f || renderPos.w > 1000.0f)
		{
			continue;
		}
		float scale = 1 / renderPos.w;
		renderPos /= renderPos.w;


		myRenderObjs.push_back(RenderObject(renderPos, finalColor, scale));

	}

	std::sort(myRenderObjs.begin(), myRenderObjs.end(), PointCloud::IsInfront);

	for (size_t i = 0; i < myRenderObjs.size(); ++i)
	{
		aSprite->SetPosition(DX2D::Vector2f(myRenderObjs[i].myPos.x + 0.5f, myRenderObjs[i].myPos.y + 0.5f));

		aSprite->SetColor({ myRenderObjs[i].myCol.x, myRenderObjs[i].myCol.y, myRenderObjs[i].myCol.z, 1.0f });
		aSprite->SetSize(DX2D::Vector2f(myRenderObjs[i].myScale * 20, myRenderObjs[i].myScale * 20));
		aSprite->Render();
	}

	myRenderObjs.clear();

	for (unsigned short i = 0; i < myChildren.Size(); i++)
	{
		myChildren[i]->Render(toWorld, aCamera, aSprite, aLight);
	}
}
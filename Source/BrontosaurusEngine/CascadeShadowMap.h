#pragma once
#include <vector>
#include <array>
#include <math.h>
#include <..\CommonUtilities\Camera.h>
#include "..\CommonUtilities\matrix44.h"


#include "RenderCamera.h"
#include "RenderPackage.h"
#include "CascadeBuffer.h"

class CRenderCamera;
class CRenderPackage;
class CModelInstance;

typedef unsigned int InstanceID;

#define NUM_FRUSTUM_CORNERS 8 // I guess, shuld it ever be something else?


class CCascadeShadowMap
{
public:
	CCascadeShadowMap(const int aNumOfCascades, const float aNear, const float aFar);
	~CCascadeShadowMap();
	void ComputeShadowProjection(const CU::Camera& aCamera);

	inline int GetNumberOfCascades();


	void Render(const CU::GrowingArray<CModelInstance*, InstanceID>& aModelList);

	const SCascade& GetCascade(const int aIndex);
	inline void SetDirection(const CU::Vector3f& aDirection);

	CRenderPackage& GetShadowMap();

	void DumpToFile(const char* aPath);
private:
	CRenderCamera myRenderCamera;
	CRenderPackage myShadowMap;

	SCascadeBuffer myCascadeBuffer;

	CU::Vector3f myDirection;
	int myNumCascades;
};

inline void CCascadeShadowMap::SetDirection(const CU::Vector3f& aDirection)
{ 
	myDirection = aDirection;
}

inline int CCascadeShadowMap::GetNumberOfCascades()
{
	return myNumCascades;
}


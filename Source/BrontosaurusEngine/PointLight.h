#pragma once
#include <vector4.h>
#include <StaticArray.h>
#include "RenderCamera.h"
struct ID3D11ShaderResourceView;

class CPointLight
{
public:

	enum ePointLightFace : char
	{
		eUP,
		eDown, 
		eLeft,
		eRight,
		eFront,
		eBack,
		eLength
	};



public:
	CPointLight(const CU::Vector4f & aColor = {0.0f, 0.0f, 0.0f, 0.0f}, const float aRange = 0.0f, const float aIntensity = 0.0f);
	~CPointLight();
	inline CU::Vector4f& GetColor();
	inline float GetIntensity();
	inline float GetRange();


	inline void SetColor(const CU::Vector4f& aColor);
	inline void SetIntensity(const float aIntensity);
	inline void SetRange(const float aRange);
	
	void RenderShadowMap(const CU::Vector4f& aPosition);

private:
	CU::StaticArray<CRenderCamera, ePointLightFace::eLength> myRenderCameras;

	ID3D11Texture2D* myTexture;
	ID3D11ShaderResourceView* myShadowCubeMap;

	CU::Vector4f myColor;

	float myIntensity;
	float myRange;
};

inline CU::Vector4f & CPointLight::GetColor()
{
	return myColor;
}

inline float CPointLight::GetIntensity()
{
	return myIntensity;
}

inline float CPointLight::GetRange()
{
	return myRange;
}

inline void CPointLight::SetColor(const CU::Vector4f & aColor)
{
	myColor = aColor;
}

inline void CPointLight::SetIntensity(const float aIntensity)
{
	myIntensity = aIntensity;
}

inline void CPointLight::SetRange(const float aRange)
{
	myRange = aRange;
}

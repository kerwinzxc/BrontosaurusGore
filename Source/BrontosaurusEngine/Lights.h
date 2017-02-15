#pragma once
#include "../Commonutilities/vector3.h"
#include "../Commonutilities/vector4.h"

#define NUMBER_OF_POINTLIGHTS 8 

enum class ePointLights
{
	eWhite,
	eSuperWhite,
	eRed,
	eBlue,
	eVoid,
	eGreen
};


namespace Lights
{
	struct SDirectionalLight
	{
		CU::Vector4f direction;
		CU::Vector4f color;
	};

	struct SPointLight
	{
		CU::Vector3f position;
		float range;
		CU::Vector3f color;
		float intensity;
	};

	struct SLightsBuffer
	{
		CU::Vector4f myCameraPos;
		SDirectionalLight myDirectionalLight;
		SPointLight myPointLights[NUMBER_OF_POINTLIGHTS];
		CU::Vector4f highlightColor;
		unsigned int myNumberOfLights;
		unsigned int cubeMap_mipCount;
		
		float trash[2];
	};

	//struct SSpotLight
	//{
	//	CU::Vector4f Direction;
	//	CU::Vector4f colour;
	//};
}
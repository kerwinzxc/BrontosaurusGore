#include "stdafx.h"
#include "LightManager.h"
#include <vector4.h>

CLightManager::CLightManager()
{

}

CLightManager::~CLightManager()
{
}

CPointLight * CLightManager::LoadLight(ePointLights aLightType)
{
	if (myPointLights.find(aLightType) == myPointLights.end()) //check if derp, else flip bool
	{
		myPointLights[aLightType] = CreateLight(aLightType);
	}

	return myPointLights.at(aLightType);
}


CPointLight * CLightManager::CreateLight(ePointLights aLightType)
{
	switch (aLightType)
	{
	case ePointLights::eRed:
	{
		CU::Vector4f color = { 1.f, 0.f, 0.f, 1.f };
		return new CPointLight(color,100.f, 1.0f);
		break;
	}
	case ePointLights::eGreen:
	{
		CU::Vector4f color = { 0.f, 1.f, 0.f, 1.f };
		return new CPointLight(color, 100.f, 1.0f);
		break;
	}
	case ePointLights::eBlue:
	{
		CU::Vector4f color = { 0.f, 0.f, 1.f, 1.f };
		return new CPointLight(color, 100.f, 1.0f);
		break;
	}
	case ePointLights::eWhite:
	{
		CU::Vector4f color = { 1.f, 1.f, 1.f, 1.f };
		return new CPointLight(color, 100.f, 1.0f);
		break;
	}
	case ePointLights::eSuperWhite:
	{
		CU::Vector4f color = { 5.f, 5.f, 5.f, 1.f };
		return new CPointLight(color, 100.f, 1.f);
		break;
	}
	case ePointLights::eVoid:
	{
		CU::Vector4f color = { 0.f, 0.f, 0.f, 0.f };
		return new CPointLight(color, 0.f, 0.f);
		break;
	}

	default:
		break;
	}

	return nullptr;
}

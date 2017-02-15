#include "stdafx.h"
#include "PointLightInstance.h"
#include "LightManager.h"
#include "Engine.h"

CPointLightInstance::CPointLightInstance()
{
	//myLight = LIGHTMANAGER->LoadLight(aType);
	myIsActive = true;
}

CPointLightInstance::~CPointLightInstance()
{
}

void CPointLightInstance::SetPosition(const CU::Vector3f & aPos)
{
	myData.position = aPos;
}

void CPointLightInstance::SetRange(const float aRange)
{
	myData.range = aRange;
}

void CPointLightInstance::SetInstensity(const float aIntencity)
{
	myData.intensity = aIntencity;
}

void CPointLightInstance::SetColor(const CU::Vector3f & aColor)
{
	myData.color = aColor;
}


const CU::Vector3f& CPointLightInstance::GetPosition() const
{
	return myData.position;
}


float CPointLightInstance::GetRange() const
{
	return myData.range;
}

float CPointLightInstance::GetInstensity() const
{
	return myData.intensity;
}

const CU::Vector3f& CPointLightInstance::GetColor() const
{
	return myData.color;
}

const Lights::SPointLight & CPointLightInstance::GetData() const
{
	return myData;
}

bool CPointLightInstance::GetIsActive() const
{
	if (myData.intensity == 0.0f || myData.range == 0.0f)
		return false;

	return myIsActive;
}

void CPointLightInstance::SetActive(const bool aIsActive)
{
	myIsActive = aIsActive;
}

#include "stdafx.h"
#include "PointLightComponent.h"
#include "PointLightInstance.h"

float fLerp(float aStart, float aEnd, float aTime)
{
	float t = aTime;
	aTime = t*t*t * (t * (6.f*t - 15.f) + 10.f);
	return (aStart + aTime*(aEnd - aStart));
}

CU::Vector3f f3Lerp(const CU::Vector3f& aStart, const CU::Vector3f& aEnd, float aTime)
{
	CU::Vector3f lerped;
	lerped.x = fLerp(aStart.x, aEnd.x, aTime);
	lerped.y = fLerp(aStart.y, aEnd.y, aTime);
	lerped.z = fLerp(aStart.z, aEnd.z, aTime);

	return lerped;
}

PointLightComponent::PointLightComponent(CScene& aScene)
	: myScene(aScene)
	, myTimeTilChangedColor(0.f)
	, myChangeColorTime(0.f)
{
	myLightID = aScene.AddPointLightInstance(CPointLightInstance());
	myType = eComponentType::ePointLight;
}

PointLightComponent::~PointLightComponent()
{
	Destroy();
}

void PointLightComponent::SetColor(const CU::Vector3f& aColor)
{
	CPointLightInstance* pointLight = myScene.GetPointLightInstance(myLightID);
	if (!pointLight) return;
	pointLight->SetColor(aColor);
	myLastColor = aColor;
}

void PointLightComponent::SetRange(const float aRange)
{
	CPointLightInstance* pointLight = myScene.GetPointLightInstance(myLightID);
	if (!pointLight) return;
	pointLight->SetRange(aRange);
	myLastRange = aRange;
}

void PointLightComponent::SetIntensity(const float aIntensity)
{
	CPointLightInstance* pointLight = myScene.GetPointLightInstance(myLightID);
	if (!pointLight) return;
	pointLight->SetInstensity(aIntensity);
	myLastIntensity = aIntensity;
}

void PointLightComponent::SetOffsetToParent(const CU::Vector3f& aOffset)
{
	myOffsetToParent = aOffset;
}

void PointLightComponent::ChangeColorOverTime(const CU::Vector3f& aColor, const float aSeconds)
{
	CPointLightInstance* pointLight = myScene.GetPointLightInstance(myLightID);
	if (!pointLight) return;
	
	if (aSeconds <= 0.f)
	{
		pointLight->SetColor(aColor);
		return;
	}

	myPreviousColor = pointLight->GetColor();
	myToBeColor = aColor;
	myTimeTilChangedColor = aSeconds;
	myChangeColorTime = aSeconds;
}

void PointLightComponent::Update(const CU::Time aDeltaTime)
{
	CPointLightInstance* pointLight = myScene.GetPointLightInstance(myLightID);
	if (!pointLight) return;

	if (myTimeTilChangedColor > 0.f && myChangeColorTime > 0.f)
	{
		myTimeTilChangedColor -= aDeltaTime.GetSeconds();
		if (myTimeTilChangedColor < 0.f)
		{
			myTimeTilChangedColor = 0.f;
			myChangeColorTime = 0.f;
			pointLight->SetColor(myToBeColor);
			return;
			//done, callback?
		}

		float percent = fLerp(1.f, 0.f, myTimeTilChangedColor / myChangeColorTime);

		pointLight->SetColor(f3Lerp(myPreviousColor, myToBeColor, percent));
	}
}

void PointLightComponent::Destroy()
{
}

void PointLightComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	CPointLightInstance* pointLight = myScene.GetPointLightInstance(myLightID);
	if (!pointLight) return;

	switch (aMessageType)
	{
	case eComponentMessageType::eObjectDone:
	case eComponentMessageType::eMoving:
	{
		pointLight->SetPosition(GetParent()->GetWorldPosition() + myOffsetToParent);
		break;
	}
	case eComponentMessageType::eTurnOnThePointLight:
		pointLight->SetActive(true);
		break;
	case eComponentMessageType::eTurnOffThePointLight:
		pointLight->SetActive(false);
		break;
	case eComponentMessageType::eSetPointLightIntensity:
		pointLight->SetInstensity(aMessageData.myFloat);
		break;
	case eComponentMessageType::eSetPointLightRange:
		pointLight->SetRange(aMessageData.myFloat);
		break;
	case eComponentMessageType::eSetPointLightColor:
		ChangeColorOverTime(aMessageData.myVector3f);
		break;
	case eComponentMessageType::eSetPointLightToLastState:
		pointLight->SetInstensity(myLastIntensity);
		pointLight->SetRange(myLastRange);
		ChangeColorOverTime(myLastColor);
		break;
	default:
		break;
	}
}

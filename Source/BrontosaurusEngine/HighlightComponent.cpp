#include "stdafx.h"
#include "HighlightComponent.h"


CHighlightComponent::CHighlightComponent()
{
	myType = eComponentType::eHighlightComponent;
}


CHighlightComponent::~CHighlightComponent()
{
}

float CHighlightComponent::GetIntensity() const
{
	return myIntensity;
}

const CU::Vector4f& CHighlightComponent::GetColor() const
{
	return myColor;
}

void CHighlightComponent::SetColor(const CU::Vector4f& aColor)
{
	myColor = aColor;
}

void CHighlightComponent::SetIntensity(float anIntensity)
{
	myIntensity = anIntensity;
}

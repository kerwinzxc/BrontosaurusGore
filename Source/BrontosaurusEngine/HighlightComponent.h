#pragma once
#include "../Components/Component.h"

class CHighlightComponent : public CComponent
{
public:
	CHighlightComponent();
	~CHighlightComponent();
	float GetIntensity() const;
	const CU::Vector4f& GetColor() const;
	void SetColor(const CU::Vector4f& aColor);
	void SetIntensity(float anIntensity);
protected:
	float myIntensity;
	CU::Vector4f myColor;
};


#pragma once
#include "Lights.h"

enum class ePointLights;
class CPointLight;

class CPointLightInstance
{
public:
	CPointLightInstance();
	~CPointLightInstance();

	void SetPosition(const CU::Vector3f& aPos);
	void SetRange(const float aRange);
	void SetInstensity(const float aIntencity);
	void SetColor(const CU::Vector3f & aColor);

	const CU::Vector3f& GetPosition() const;
	float GetRange() const;
	float GetInstensity() const;
	const CU::Vector3f& GetColor() const;

	const Lights::SPointLight& GetData() const;

	bool GetIsActive() const;
	void SetActive(const bool aIsActive);
private:
	Lights::SPointLight myData;
	bool myIsActive;
};


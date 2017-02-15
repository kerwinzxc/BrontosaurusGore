#pragma once
#include "Component.h"
#include "Lights.h"
#include "Scene.h"

class PointLightComponent : public CComponent
{
public:
	PointLightComponent(CScene& aScene);
	~PointLightComponent();

	void SetColor(const CU::Vector3f& aColor);
	void SetRange(const float aRange);
	void SetIntensity(const float aIntensity);
	void SetOffsetToParent(const CU::Vector3f& aOffset);

	void ChangeColorOverTime(const CU::Vector3f& aColor, const float aSeconds = 0.5f);
	void Update(const CU::Time aDeltaTime);

	void Destroy() override;
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:
	CU::Vector3f myOffsetToParent;
	CU::Vector3f myToBeColor;
	CU::Vector3f myPreviousColor;
	CScene& myScene;
	InstanceID myLightID;

	float myChangeColorTime;
	float myTimeTilChangedColor;

	CU::Vector3f myLastColor;
	float myLastRange;
	float myLastIntensity;
};


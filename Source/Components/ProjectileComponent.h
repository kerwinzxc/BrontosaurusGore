#pragma once
#include "Component.h"

struct ProjectileData;
class ProjectileComponent : public CComponent
{
public:
	ProjectileComponent();
	~ProjectileComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Destroy() override;

	void Activate(ProjectileData* someData, const CU::Vector3f& aDirection, const CU::Vector3f& aPosition);
	void Update(float aDeltaTime);
private:
	ProjectileData* myData;
	CU::Vector3f myDirection;
	bool myIsActive;
};


#pragma once
#include "Component.h"
#include "InstanceID.h"

class CScene;

class CFireComponent : public CComponent
{
public:
	CFireComponent(CScene& aScene, InstanceID aFireEmitterID);
	~CFireComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Destroy() override;

private:
	CScene& myScene;
	const InstanceID myID;
};

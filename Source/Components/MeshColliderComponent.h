#pragma once
#include "ColliderComponent.h"

struct SMeshColliderData : public SColliderData
{
	SMeshColliderData() { myType = eColliderType::eMesh; }
	const char* myPath;
};

class CMeshColliderComponent : public CColliderComponent
{
public:
	CMeshColliderComponent(const SMeshColliderData& aColliderData, Physics::CPhysicsScene* aScene);
	~CMeshColliderComponent();

	const SMeshColliderData* GetData() override { return &myData; };
	inline SColliderData::eColliderType GetType() { return SColliderData::eColliderType::eMesh; }
	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;

private:
	void Init(const CU::Vector3f& aScale);
	SMeshColliderData myData;
	Physics::CPhysicsScene* myScene;
};


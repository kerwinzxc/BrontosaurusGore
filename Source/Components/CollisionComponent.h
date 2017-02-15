#pragma once
#include "Component.h"

class ICollider;
enum eColliderType : unsigned int;

class CCollisionComponent : public CComponent
{
public:
	CCollisionComponent(ICollider* aCollider);
	CCollisionComponent(const CCollisionComponent& aCopy) = delete;
	~CCollisionComponent();

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Destroy() override;

	void SetColliderType(const eColliderType aColliderType);
	void AddCollidsWith(const unsigned int aColliderTypes);

	void ActivateCollider();
	void DeactivateCollider();

	inline ICollider* GetCollider();

private:
	void OnCollisionEnter(ICollider* aCollider);
	void OnCollisionExit(ICollider* aCollider);
	
	ICollider* myCollider;
};

inline ICollider* CCollisionComponent::GetCollider()
{
	return myCollider;
}

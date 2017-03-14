#pragma once
#include "Component.h"
#include "..\Physics\PhysicsActor.h"
#include "..\Physics\IPhysicsCallback.h"

namespace Physics
{
	class CPhysicsActor;
}

struct SColliderData 
{
	enum class eColliderType
	{
		eNone,
		eBox,
		eSphere,
		eCapsule,
		eRigidbody,
		eMesh
	} myType = eColliderType::eNone;
	CU::Vector3f material;
	CU::Vector3f center;

	bool IsTrigger = false;
};
class CColliderComponentManager;

class CColliderComponent : public CComponent , public Physics::IPhysicsCallback
{
public:
	CColliderComponent(SColliderData* aColliderData, Physics::CShape* aShape, Physics::CPhysicsActor* aActor);
	virtual ~CColliderComponent();
	void UpdatePosition();

	virtual void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	virtual inline SColliderData::eColliderType GetType() = 0;
	virtual const SColliderData* GetData() { return nullptr; }
	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;

	inline Physics::CShape* GetShape() { return myShape; }
	inline Physics::CPhysicsActor* GetActor() { return myActor; }
	
	inline void SetManager(CColliderComponentManager* aManager) { myManager = aManager; }
	inline CColliderComponentManager* GetManager() { return myManager; }

	void OnTriggerEnter(const IPhysicsCallback* aOther) override;
	void OnTriggerExit(const IPhysicsCallback* aOther) override;
	void OnCollisionEnter(const IPhysicsCallback* aOther) override;
	void OnCollisionExit(const IPhysicsCallback* aOther) override;

protected:

	Physics::CShape* myShape;
	Physics::CPhysicsActor* myActor;
private:
	SColliderData myData;
	CColliderComponentManager* myManager;
};


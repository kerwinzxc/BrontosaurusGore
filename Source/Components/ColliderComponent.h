#pragma once
#include "Component.h"
#include "..\Physics\PhysicsActor.h"
#include "..\Physics\IPhysicsCallback.h"
#include "..\Physics\CollisionLayers.h"

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

	Physics::ECollisionLayer myLayer = Physics::ECollisionLayer::eDefault;
	Physics::ECollisionLayer myCollideAgainst = static_cast<Physics::ECollisionLayer>(Physics::CollideEverything);

	bool IsTrigger = false;
};

struct SStoredCallBackData
{
	void* componentPtr;
	eComponentMessageType collisionType;
};

class CColliderComponentManager;

class CColliderComponent : public CComponent , public Physics::IPhysicsCallback
{
public:
	CColliderComponent(const SColliderData& aColliderData, Physics::CShape* aShape, Physics::CPhysicsActor* aActor);
	virtual ~CColliderComponent();
	void UpdatePosition();
	void UpdateCallbacks();

	virtual void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	virtual inline SColliderData::eColliderType GetType() = 0;
	virtual const SColliderData* GetData() { return nullptr; }
	bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData) override;

	inline Physics::CShape* GetShape() { return myShape; }
	inline Physics::CPhysicsActor* GetActor() { return myActor; }
	
	inline void SetManager(CColliderComponentManager* aManager) { myManager = aManager; }
	inline CColliderComponentManager* GetManager() { return myManager; }

	void OnTriggerEnter(Physics::CPhysicsCallbackActor* aOther) override;
	void OnTriggerExit(Physics::CPhysicsCallbackActor* aOther) override;
	void OnCollisionEnter(Physics::CPhysicsCallbackActor* aOther) override;
	void OnCollisionExit(Physics::CPhysicsCallbackActor* aOther) override;

protected:
	Physics::CShape* myShape;
	Physics::CPhysicsActor* myActor;
	SColliderData myData;
private:
	CU::GrowingArray<SStoredCallBackData> myStoredCallBackDataList;
	CColliderComponentManager* myManager;
};


#pragma once

class CColliderComponent;


enum class eComponentQuestionType
{
	eHasCameraComponent,
	eGetCameraLookat,
	eGetCameraPosition,
	eGetCollisionShape,
	eMovePhysicsController,
	ePhysicsControllerGrounded,
	eLength,
};

struct SComponentQuestionData
{
	SComponentQuestionData() : myVector3f() {}

	union
	{
		CColliderComponent* myCollider;
		CU::Vector4f myVector4f;
		CU::Vector3f myVector3f;

		bool myBool;
	};
};
#pragma once

class CColliderComponent;


enum class eComponentQuestionType
{
	eHasCameraComponent,
	eGetCameraLookat,
	eGetCameraPosition,
	eGetAmmoLeftString,
	eGetCameraObject,
	eGetCollisionShape,
	eMovePhysicsController,
	ePhysicsControllerConstraints,
	eLength,
};

struct SAmmoLeftData
{
	const char* weaponName;
	unsigned short ammoLeft;
	unsigned short maxAmmo;
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
		char myChar;

		const char* myString;
		SAmmoLeftData* myAmmoLeftData;
		CGameObject* myGameObject;
	};
};

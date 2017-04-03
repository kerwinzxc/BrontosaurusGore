#pragma once

class CColliderComponent;


enum class eComponentQuestionType
{
	eHasCameraComponent,
	eGetCameraLookat,
	eGetHealth,
	eGetMaxHealth,
	eGetArmor,
	eGetMaxArmor,
	eGetCameraPosition,
	eGetAmmoLeftString,
	eGetCameraObject,
	eGetCollisionShape,
	eMovePhysicsController,
	ePhysicsControllerConstraints,
	eCanShoot,
	eEnemyNetworkID,
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
		CU::Vector4f myVector4f;
		CU::Vector3f myVector3f;
		CColliderComponent* myCollider;

		int myInt;

		bool myBool;
		char myChar;

		const char* myString;
		SAmmoLeftData* myAmmoLeftData;
		CGameObject* myGameObject;
	};
};

#pragma once

class CColliderComponent;
class CWeapon;
struct SWeaponPickupData;

namespace CU
{
	template<typename ObjectType, typename SizeType = unsigned int, bool USE_SAFE_MODE = true>
	class GrowingArray;
}

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
	eHasDeathAnimation,
	eGetWeaponFactoryIndexOfActiveWeapon,
	eEnemyNetworkID,
	eGetCurrentAmmoData,
	eGetWeapons,
	eIsEnemy,
	eHaveTriggerCollision,
	eGetWeaponFactoryIndexOfSpecificWeapon,

	eHitPosition,
	eLastHitNormal,
	eLastHitPosition,
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
	SComponentQuestionData();

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
		const CU::GrowingArray<CWeapon*>* myWeapons;
		SWeaponPickupData* myWeaponPickupData;
	};
};

inline SComponentQuestionData::SComponentQuestionData(): myVector3f()
{
}

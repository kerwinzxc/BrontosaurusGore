#pragma once
#include "../CommonUtilities/matrix33.h"
#include "../CommonUtilities/matrix44.h"

class CPath;
class ICollider;
class CComponent;
class CGameObject;
struct SAmmoData;
struct SAmmoCheckData;
struct SAmmoReplenishData;
class CWeapon;
struct SExplosionData;
enum class ePlayerControls;

namespace std
{
	template<class _Fty>
	class function;
}

namespace CU
{
	template<typename ObjectType, typename SizeType = unsigned int, bool USE_SAFE_MODE = true>
	class GrowingArray;

	template<typename TYPE>
	class Matrix44;
	using Matrix44f = Matrix44<float>;

	template<typename TYPE>
	class Matrix33;
	using Matrix33f = Matrix33<float>;

	template<typename TYPE>
	class Vector3;
	using Vector3f = Vector3<float>;

	template<typename TYPE>
	class Vector2;
	using Vector2f = Vector2<float>;
}
enum class eComponentType : unsigned char;


enum class eComponentMessageType
{
	eAddComponent,
	ePlaySound,
	eMoving,
	eAddArmor,
	eSetVisibility,
	eSetIsColliderActive,
	eObjectDone,
	eAddMessageSubscription,
	eSetHighlight,
	eTurnOnThePointLight,
	eTurnOffThePointLight,
	eSetPointLightIntensity,
	eSetPointLightRange,
	eSetPointLightColor,
	eSetPointLightToLastState,
	eKeyPressed,
	eKeyReleased,
	ePitch,
	eTryToShoot,
	eCheckIfHaveAmmoForShooting,
	eShoot,
	eShootWithNetworking,
	eAddWeapon,
	eAddNewAmmoType,
	eAddWeaponWithoutChangingToIt,
	eWeaponFactoryGiveWeaponToWeaponSystem,
	eGiveAmmo,
	eRemoveAmmo,
	//eSetDirectionForShooting,
	eChangeWeapon,
	eTakeDamage,
	eHeal,
	eTookDamage,
	eDied,
	eStopJumping,
	eSelectWeapon,
	eOnTriggerEnter,
	eOnTriggerExit,
	eOnCollisionEnter,
	eOnCollisionExit,
	eCheckPointReset,
	eSetControllerPosition,
	eActivateExplosion,
	eActivate,
	eDeactivate,
	eNetworkDoDamage,
	eActivateEmitter,
	eServerShoot,
	eAddForce,
	eNullifyForces, // doesn't do stuff :(
	eServerChangeWeapon,
	eAddWeaponIndex,
	eImpStartToJump,
	eEquip,
	eUnequip,
	eLength,
};

struct SComponentMessageData
{
	SComponentMessageData() : myVoidPointer(nullptr) {}

	union
	{
		void* myVoidPointer;
		struct
		{
			CComponent* myComponent;
			eComponentType myComponentTypeAdded;
		};
		
		struct  
		{
			bool myPhysicsUpdated;
			bool myBool;
		};
		CGameObject* myGameObject;
		struct SComponentMessageCallback* myComponentMessageCallback;
		ePlayerControls myPlayerControl;
		int myInt;
		unsigned short myUShort;
		unsigned char myUChar;
		const char* myString;
		float myFloat;
		CU::Vector2f myVector2f;
		CU::Vector3f myVector3f;
		CU::Vector4f myVector4f;

		ICollider* myCollider;

		SAmmoData* myAmmoData;
		SAmmoCheckData* myAmmoCheckData;
		SAmmoReplenishData* myAmmoReplenishData;
		const SExplosionData* myExplosionData;
		CWeapon* myWeapon;
		const std::function<void(void)>* myVoidFunction;
	};
};

static_assert(sizeof(SComponentMessageData) <= sizeof(CU::Vector4f), "Don't put so big things in the component message data :((");

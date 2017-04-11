#pragma once
#include "WeaponSoundData.h"
#include "../Audio/AudioInterface.h"

namespace CU {
	class CJsonValue;
}

struct SWeaponData;
struct SAmmoData;
class CWeapon;
class CGameObject;
class CWeaponSystemComponent;
class CGameObjectManager;
class CModelComponentManager;

namespace Physics
{
	class CPhysicsScene;
}

class CWeaponFactory
{
public:
	CWeaponFactory();
	~CWeaponFactory();

	void Init(CGameObjectManager* aGameObjectPointer, CModelComponentManager* aModelComponentManager, Physics::CPhysicsScene* aPhysicsScene);
	void LoadWeapons();
	unsigned short CreateWeapon(const char* aWeaponName, CGameObject* aObjectToGiveAWeaponTo);
	void CreateWeapon(unsigned short aWeaponIndex, CGameObject* aObjectToGiveAWeaponTo);
	void CreateWeapon(const char* aWeaponName, CWeaponSystemComponent* aWeaponSystemToGiveAWeaponTo);
	short GetWeaponFactoryWeaponIndex(const char* aWeaponName);
	SWeaponData* GetWeaponDataFromIndex(unsigned int aIndex);
	void MakeWeaponModel(CGameObject* aOwner, CWeapon* aWeapon);
	void LoadWeaponsFromJsonValue(const CU::CJsonValue& aJsonValue, bool aIsPlayerWeapon);
private:
	SWeaponSoundData GetSoundData(CU::CJsonValue aSoundData);
	CU::GrowingArray<SWeaponData*> myWeaponDataList;
	CU::GrowingArray<SAmmoData*> myAmmoDataList;
	CU::GrowingArray<CWeapon*> myWeaponList;
	CGameObjectManager* myGameObjectManagerPointer;
	CModelComponentManager* myModelComponentManagerPointer;
	Physics::CPhysicsScene* myPhysicsScene;
};


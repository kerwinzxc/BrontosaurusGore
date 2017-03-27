#pragma once
#include "WeaponSoundData.h"

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
	void CreateWeapon(const char* aWeaponName, CGameObject* aObjectToGiveAWeaponTo);
	void CreateWeapon(const char* aWeaponName, CWeaponSystemComponent* aWeaponSystemToGiveAWeaponTo);
	void MakeWeaponModel(CGameObject* aOwner, CWeapon* aWeapon);
private:
	SWeaponSoundData GetSoundData(CU::CJsonValue aSoundData);
	CU::GrowingArray<SWeaponData*> myWeaponDataList;
	CU::GrowingArray<SAmmoData*> myAmmoDataList;
	CU::GrowingArray<CWeapon*> myWeaponList;
	CGameObjectManager* myGameObjectManagerPointer;
	CModelComponentManager* myModelComponentManagerPointer;
	Physics::CPhysicsScene* myPhysicsScene;
};


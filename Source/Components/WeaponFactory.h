#pragma once
struct SWeaponData;
struct SAmmoData;
class CWeapon;
class CGameObject;
class CWeaponSystemComponent;
class CGameObjectManager;
class CModelComponentManager;
class CWeaponFactory
{
public:
	CWeaponFactory();
	~CWeaponFactory();

	void Init(CGameObjectManager* aGameObjectPointer, CModelComponentManager* aModelComponentManager);
	void LoadWeapons();
	void CreateWeapon(const char* aWeaponName, CGameObject* aObjectToGiveAWeaponTo);
	void CreateWeapon(const char* aWeaponName, CWeaponSystemComponent* aWeaponSystemToGiveAWeaponTo);
	void MakeWeaponModel(CGameObject* aOwner, SWeaponData* someWeaponData);
private:
	CU::GrowingArray<SWeaponData*> myWeaponDataList;
	CU::GrowingArray<SAmmoData*> myAmmoDataList;
	CGameObjectManager* myGameObjectManagerPointer;
	CModelComponentManager* myModelComponentManagerPointer;
};


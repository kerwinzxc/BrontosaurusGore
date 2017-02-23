#pragma once
class CWeaponSystemComponent;
class CWeaponFactory;
class CWeaponSystemManager
{
public:
	CWeaponSystemManager(CWeaponFactory* aWeaponFactoryPointer);
	~CWeaponSystemManager();
	CWeaponSystemComponent* CreateAndRegisterComponent();
	void Update(const CU::Time& aDeltaTime);

private:
	CU::GrowingArray<CWeaponSystemComponent*> myComponents;
	CWeaponFactory* myWeaponFactoryPointer;
};


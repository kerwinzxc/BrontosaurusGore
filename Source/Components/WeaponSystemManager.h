#pragma once
class WeaponSystemComponent;
class WeaponFactory;
class WeaponSystemManager
{
public:
	WeaponSystemManager(WeaponFactory* aWeaponFactoryPointer);
	~WeaponSystemManager();
	WeaponSystemComponent* CreateAndRegisterComponent();
	void Update(const CU::Time& aDeltaTime);

private:
	CU::GrowingArray<WeaponSystemComponent*> myComponents;
	WeaponFactory* myWeaponFactoryPointer;
};


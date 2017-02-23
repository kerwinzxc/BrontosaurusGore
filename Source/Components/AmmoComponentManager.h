#pragma once
class CAmmoComponent;
class CAmmoComponentManager
{
public:
	CAmmoComponentManager();
	~CAmmoComponentManager();
	CAmmoComponent* CreateAndRegisterComponent();
	void Update(const CU::Time& aDeltaTime);

private:
	CU::GrowingArray<CAmmoComponent*> myComponents;
};


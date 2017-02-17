#pragma once
class AmmoComponent;
class AmmoComponentManager
{
public:
	AmmoComponentManager();
	~AmmoComponentManager();
	AmmoComponent* CreateAndRegisterComponent();
	void Update(const CU::Time& aDeltaTime);

private:
	CU::GrowingArray<AmmoComponent*> myComponents;
};


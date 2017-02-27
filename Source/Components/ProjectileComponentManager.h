#pragma once
class CProjectileComponent;
class CProjectileComponentManager
{
public:
	CProjectileComponentManager();
	~CProjectileComponentManager();
	CProjectileComponent* CreateAndRegisterComponent();
	void Update(const CU::Time& aDeltaTime);

private:
	CU::GrowingArray<CProjectileComponent*> myComponents;
};


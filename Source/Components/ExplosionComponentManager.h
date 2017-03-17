#pragma once
class CExplosionComponent;
class CExplosionComponentManager
{
public:
	CExplosionComponentManager();
	~CExplosionComponentManager();
	CExplosionComponent* CreateAndRegisterComponent();
	void Update(const CU::Time& aDeltaTime);

private:
	CU::GrowingArray<CExplosionComponent*> myComponents;
};

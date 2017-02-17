#pragma once
class ProjectileComponent;
class ProjectileComponentManager
{
public:
	ProjectileComponentManager();
	~ProjectileComponentManager();
	ProjectileComponent* CreateAndRegisterComponent();
	void Update(const CU::Time& aDeltaTime);

private:
	CU::GrowingArray<ProjectileComponent*> myComponents;
};


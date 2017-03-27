#pragma once
class CDamageOnCollisionComponent;
class CDamageOnCollisionComponentManager
{
public:
	CDamageOnCollisionComponentManager();
	~CDamageOnCollisionComponentManager();

	CDamageOnCollisionComponent* CreateAndRegisterComponent();
	void Update(const CU::Time& aDeltaTime);
private:
	CU::GrowingArray<CDamageOnCollisionComponent*> myComponents;
};


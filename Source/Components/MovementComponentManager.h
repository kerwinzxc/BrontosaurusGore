#pragma once

class CMovementComponent;

class CMovementComponentManager
{
public:
	CMovementComponentManager();
	~CMovementComponentManager();

	CMovementComponent* CreateAndRegisterComponent();

	void Update(CU::Time aDeltaTime);

private:
	CU::GrowingArray<CMovementComponent*> myComponents;
};


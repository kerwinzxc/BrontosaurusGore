#pragma once
class CInputComponent;
class CInputComponentManager
{
public:
	CInputComponentManager();
	~CInputComponentManager();

	CInputComponent* CreateAndRegisterComponent();
	void Update();

private:
	CU::GrowingArray<CInputComponent*> myComponents;
};


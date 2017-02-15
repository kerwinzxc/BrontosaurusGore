#pragma once

class PointLightComponent;
class CScene;

class CPointLightComponentManager
{
public:
	static void Destroy();
	static void Create(CScene& aScene);
	static CPointLightComponentManager& GetInstance();

	PointLightComponent* CreateAndRegisterComponent();
	void Update(const CU::Time aDeltaTime);

private:
	CPointLightComponentManager(CScene& aScene);
	~CPointLightComponentManager();


private:
	static CPointLightComponentManager* ourInstance;
	
	CU::GrowingArray<PointLightComponent*> myComponents;
	CScene& myScene;
};


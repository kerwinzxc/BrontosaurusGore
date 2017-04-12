#pragma once

class PointLightComponent;
class CScene;

namespace Lights
{
	struct SDirectionalLight;
}

class CPointLightComponentManager
{
public:
	static void Destroy();
	static void Create(CScene& aScene);
	static CPointLightComponentManager& GetInstance();

	PointLightComponent* CreateAndRegisterComponent();
	void Update(const CU::Time aDeltaTime);

	void AddDirectionalLightToScene(const Lights::SDirectionalLight& aDirectionalLight);

private:
	CPointLightComponentManager(CScene& aScene);
	~CPointLightComponentManager();


private:
	static CPointLightComponentManager* ourInstance;
	
	CU::GrowingArray<PointLightComponent*> myComponents;
	CScene& myScene;
};


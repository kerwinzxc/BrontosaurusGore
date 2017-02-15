#pragma once

class CComponent;
class CScriptComponent;

class CScriptComponentManager
{
public:
	CScriptComponentManager();
	~CScriptComponentManager();

	CComponent* CreateAbstractComponent(const std::string& aScriptPath);
	CScriptComponent* CreateComponent(const std::string& aScriptPath);
	void DestroyComponent(CScriptComponent* aScriptComponent);

	static CScriptComponentManager* GetInstance();

private:
	CU::GrowingArray<CScriptComponent*> myComponents;

	static CScriptComponentManager* ourInstance;
};


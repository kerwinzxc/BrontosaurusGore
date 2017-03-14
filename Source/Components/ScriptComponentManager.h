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

	CComponent* CreateAbstractComponent(const std::string& aScriptPath, const std::map<std::string, std::string>& aDataMap);
	CScriptComponent* CreateComponent(const std::string& aScriptPath, const std::map<std::string, std::string>& aDataMap);

	void DestroyComponent(CScriptComponent* aScriptComponent);

	static CScriptComponentManager* GetInstance();

private:
	CU::GrowingArray<CScriptComponent*> myComponents;

	static CScriptComponentManager* ourInstance;
};

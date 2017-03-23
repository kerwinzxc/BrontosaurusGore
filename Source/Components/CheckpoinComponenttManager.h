#pragma once
#include <unordered_map>
class CCheckPointComponent;
class CCheckpoinComponentManager
{
public:
	~CCheckpoinComponentManager();
	CCheckPointComponent* CreateAndRegisterComponent();
	static void CreateInstance();
	static CCheckpoinComponentManager* GetInstance();
	static void DestoryInstance();
	inline CCheckPointComponent* GetComponent(const unsigned char aID);
private:
	CCheckpoinComponentManager();

private:
	static CCheckpoinComponentManager* ourInstance;
	std::unordered_map<unsigned char, CCheckPointComponent*> myCheckPointComponentList;
};

inline CCheckPointComponent* CCheckpoinComponentManager::GetComponent(const unsigned char aID)
{
	return myCheckPointComponentList.at(aID);
}
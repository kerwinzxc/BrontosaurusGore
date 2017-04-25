#pragma once
#include <unordered_map>
class CCheckPointComponent;
class CCheckpointComponentManager
{
public:
	~CCheckpointComponentManager();
	CCheckPointComponent* CreateAndRegisterComponent(short aIndex);
	static void CreateInstance();
	static CCheckpointComponentManager* GetInstance();
	static void DestoryInstance();
	inline CCheckPointComponent* GetComponent(const unsigned char aID);
	const CU::Vector3f GetNextCheckpointPosition();
private:
	CCheckpointComponentManager();

private:
	static CCheckpointComponentManager* ourInstance;
	std::unordered_map<unsigned char, CCheckPointComponent*> myCheckPointComponentList;
};

inline CCheckPointComponent* CCheckpointComponentManager::GetComponent(const unsigned char aID)
{
	return myCheckPointComponentList.at(aID);
}
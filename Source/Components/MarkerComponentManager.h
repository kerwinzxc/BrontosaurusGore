#pragma once
class CMarkerComponent;
class CMarkerComponentManager
{
public:
	~CMarkerComponentManager();

	static void CreateInstance();
	static CMarkerComponentManager* GetInstance();
	static void DestroyInstance();
	void Update(float aDeltaTime);
	inline void SetMarkerObject(CMarkerComponent* aMarkerComponent);
private:
	CMarkerComponentManager();

private:
	static CMarkerComponentManager* ourInstance;
	CMarkerComponent* myMarkerComponent;
};

inline void CMarkerComponentManager::SetMarkerObject(CMarkerComponent* aMarkerComponent)
{
	myMarkerComponent = aMarkerComponent;
}
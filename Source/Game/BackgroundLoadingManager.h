#pragma once
#include "PlayState.h"

class CBackgroundLoadingManager
{
public:
	static void CreateInstance();//why
	static CBackgroundLoadingManager& GetInstance();//WHY
	static void DestroyInstance(); //THIS ENGINE IS BROKEN WE SAID A FEW SINGLETONS!!!!!!!!!!!!!!!!!

	void CreateStateToLoad(StateStack& aStateStack, const int aLevelIndex, const bool aShouldReturnToLevelSelect = false);
	CPlayState* GetPlaystate();

	bool GetIfActive();
	int GetLevelIndex()const;

	void LoadAnotherState(CPlayState* aPlayState);

	void Clear();
private:
	CBackgroundLoadingManager();
	~CBackgroundLoadingManager();

	void InternalLoadAnotherState(CPlayState* aPlayState);
	static void DeleteState(CPlayState* aPlayState);

	CPlayState* myCurrentPlayState;

	int myLevelIndex;

	static CBackgroundLoadingManager* ourInstance;
};

inline int CBackgroundLoadingManager::GetLevelIndex() const
{
	return myLevelIndex;
}

inline bool CBackgroundLoadingManager::GetIfActive()
{
	return myCurrentPlayState != nullptr;
}

inline void CBackgroundLoadingManager::DeleteState(CPlayState* aPlayState)
{
	delete aPlayState;
}

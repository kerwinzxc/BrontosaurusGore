#pragma once
#include "../Game/PlayState.h"

class CScene;

// GET_LOADMANAGER(LOCAL_VARIABLE) defined below

class LoadManager
{
public:
	friend class LoadManagerGuard;

	static LoadManager* GetInstance();

	CScene& GetCurrentScene() const;
	CPlayState& GetCurrentPLaystate() const;

	void RegisterFunctions();

private:
	static void CreateInstance(CPlayState& aPlayState, CScene& aScene);
	static void DestroyInstance();

	LoadManager(CPlayState& aPlayState, CScene& aScene);
	~LoadManager();

	static LoadManager* ourInstance;

	CScene& myCurrentScene;
	CPlayState& myCurrentPlaystate;
};

class LoadManagerGuard
{
public:
	~LoadManagerGuard()
	{
		LoadManager::DestroyInstance();
	}

private:
	LoadManagerGuard(CPlayState& aPlayState, CScene& aScene)
	{
		LoadManager::CreateInstance(aPlayState, aScene);
	}

	friend void CPlayState::Load();
};

#define GET_LOADMANAGER(LOCAL_VARIABLE)									\
LoadManager* LOCAL_VARIABLE_invisiblePtr = LoadManager::GetInstance();	\
if (LOCAL_VARIABLE_invisiblePtr == nullptr) { return NULL_COMPONENT; }	\
LoadManager& LOCAL_VARIABLE = *LOCAL_VARIABLE_invisiblePtr
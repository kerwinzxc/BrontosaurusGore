#pragma once
#include "Game/PlayState.h"

class CScene;

#define LOAD_MGR LoadManager::GetInstance()

class LoadManager
{
public:
	friend class LoadManagerGuard;

	static LoadManager& GetInstance();


	void SetCurrentScene(CScene* aScene);
	CScene* GetCurrentScene() const;

	void SetCurrentPlayState(CPlayState* aPlaystate);
	CPlayState* GetCurrentPLaystate() const;

	void RegisterFunctions();

private:
	static void CreateInstance();
	static void DestroyInstance();

	LoadManager();
	~LoadManager();

	static LoadManager* ourInstance;

	CScene* myCurrentScene;
	CPlayState* myCurrentPlaystate;
};

class LoadManagerGuard
{
public:
	~LoadManagerGuard()
	{
		LoadManager::DestroyInstance();
	}

private:
	LoadManagerGuard()
	{
		LoadManager::CreateInstance();
	}

	friend void CPlayState::Load();
};
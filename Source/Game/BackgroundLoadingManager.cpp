#include "stdafx.h"
#include "BackgroundLoadingManager.h"
#include "Work.h"
#include "Engine.h"
#include "ThreadPool.h"

CBackgroundLoadingManager* CBackgroundLoadingManager::ourInstance = nullptr;

CBackgroundLoadingManager::CBackgroundLoadingManager(): myCurrentPlayState(nullptr), myLevelIndex(0)
{
}

CBackgroundLoadingManager::~CBackgroundLoadingManager()
{
}

void CBackgroundLoadingManager::CreateInstance()
{
	ourInstance = new CBackgroundLoadingManager;
}

CBackgroundLoadingManager& CBackgroundLoadingManager::GetInstance()
{
	return *ourInstance;
}

void CBackgroundLoadingManager::DestroyInstance()
{
	delete ourInstance;
	ourInstance = nullptr;
}

void CBackgroundLoadingManager::CreateStateToLoad(StateStack& aStateStack, const int aLevelIndex, const bool aShouldReturnToLevelSelect)
{
	myLevelIndex = aLevelIndex;
	myCurrentPlayState = new CPlayState(aStateStack, aLevelIndex, aShouldReturnToLevelSelect);

	CU::Work myWork(std::bind(&CPlayState::Load, myCurrentPlayState));
	CEngine::GetInstance()->GetThreadPool()->AddWork(myWork);
}

CPlayState* CBackgroundLoadingManager::GetPlaystate()
{
	return myCurrentPlayState;
}

void CBackgroundLoadingManager::LoadAnotherState(CPlayState* aPlayState)
{
	CU::Work work(std::bind(&CBackgroundLoadingManager::InternalLoadAnotherState, this, aPlayState));
	CEngine::GetInstance()->GetThreadPool()->AddWork(work);
}

void CBackgroundLoadingManager::Clear()
{
	myCurrentPlayState = nullptr;
}

void CBackgroundLoadingManager::InternalLoadAnotherState(CPlayState* aPlayState)
{
	while(myCurrentPlayState->IsLoaded() == false){}

	delete myCurrentPlayState;

	aPlayState->Load();
}

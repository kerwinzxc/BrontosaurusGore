#include "stdafx.h"
#include "LoadState.h"
#include "PlayState.h"
#include "Engine.h"
#include "ThreadPool.h"
#include "SpriteInstance.h"
#include "StateStack/StateStack.h"

#include "Renderer.h"
#include "BackgroundLoadingManager.h"
#include "Game/Game.h"

CLoadState::CLoadState(StateStack& aStateStack, const int aLevelIndex, const bool aShouldReturnToLevelSelect): State(aStateStack), myLevelIndex(aLevelIndex), myShouldReturnToLevelSelect(aShouldReturnToLevelSelect)
{
	myPlayState = nullptr;
}

CLoadState::~CLoadState()
{
}

void CLoadState::Init()
{
	//RENDERER.ClearRenderQueue();
	CBackgroundLoadingManager &bLM = CBackgroundLoadingManager::GetInstance();

	if (bLM.GetIfActive() == true)
	{
		if (bLM.GetLevelIndex() == myLevelIndex)
		{
			myPlayState = bLM.GetPlaystate();
		}
		else
		{
			myPlayState = new CPlayState(myStateStack, myLevelIndex, myShouldReturnToLevelSelect);
			bLM.LoadAnotherState(myPlayState);
		}
	}
	else
	{
		bLM.CreateStateToLoad(myStateStack, myLevelIndex, myShouldReturnToLevelSelect);
		myPlayState = bLM.GetPlaystate();
	}
	myLoadingAnimation.Init(new CSpriteInstance("Sprites/LoadingScreen/Loading.dds", { 256.f / WINDOW_SIZE.x, 64.f / WINDOW_SIZE.y }));
}

eStateStatus CLoadState::Update(const CU::Time& aDeltaTime)
{
	myLoadingAnimation.Update(aDeltaTime);

	if (myPlayState->IsLoaded() == true)
	{
		myStateStack.SwapState(myPlayState);
		CBackgroundLoadingManager::GetInstance().Clear();
	}

	return eStateStatus::eKeep;
}

void CLoadState::Render()
{
	SChangeStatesMessage msg;

	msg.myBlendState = eBlendState::eAlphaBlend;
	msg.myDepthStencilState = eDepthStencilState::eDisableDepth;
	msg.myRasterizerState = eRasterizerState::eDefault;
	msg.mySamplerState = eSamplerState::eClamp;


	RENDERER.AddRenderMessage(new SChangeStatesMessage(msg));
	myLoadingAnimation.Render();

}

void CLoadState::OnEnter(const bool /*aLetThroughRender*/)
{
}

void CLoadState::OnExit(const bool /*aLetThroughRender*/)
{
}

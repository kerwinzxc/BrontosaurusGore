#pragma once
#include "../StateStack/StateStack.h"
#include "../TClient/Client.h"
#include "GameEventMessenger.h"

namespace CU
{
	class Time;
}

class CSplashScreenState;

class CGame
{
public:
	CGame();
	~CGame();

	void Init();
	bool Update(const CU::Time& aDeltaTime);
	void Render();

	void CleanUpNow();

private:
	CSplashScreenState* mySplashScreenState;
	StateStack myStateStack;
	CClient myClient;
	CGameEventMessenger myGameEventMessenger;
};

#pragma once
#include "Event.h"

class PushState : public Event
{
public:
	enum class eState
	{
		ePlayState,
		eDeathScreen,
		eCreditScreen,
		//eOptionsMenu,
		eLevelEndScreen,
		ePauseScreen,
		eLevelSelect,
		eSplashScreen,
		eHatShop,
		eInGameCredits,
		eDialog
	};

	static const int _DONT_ASK_ = 0; //Vad fan e detta? Ja jag vet att jag inte ska fråga men ändå.

	PushState(const eState aState, const int aLevelIndex);
	~PushState();

	eMessageReturn DoEvent(StateStack* aStateStack) const override;

private:
	const eState myState;
	const int myLevelIndex;
};


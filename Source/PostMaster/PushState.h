#pragma once
#include "Event.h"
#include "../ThreadedPostmaster/Message.h"

class PushState : public Postmaster::Message::IMessage
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
		eMenu,
		eDialog,
	};

	static const int _DONT_ASK_ = 0; //Vad fan e detta? Ja jag vet att jag inte ska fr�ga men �nd�.

	PushState(const eState aState, const int aLevelIndex);
	~PushState();

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;
	eState GetStateType() const;
	int GetLevelIndex() const;
	IMessage* Copy() override;
private:
	const eState myState;
	const int myLevelIndex;
};


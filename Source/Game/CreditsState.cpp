#include "stdafx.h"
#include "CreditsState.h"

#include "PostMaster\PostMaster.h"
#include "PostMaster\Message.h"
#include "PostMaster\PopCurrentState.h"

#include "BrontosaurusEngine\ModelInstance.h"
#include "BrontosaurusEngine\SpriteInstance.h"

#include "BrontosaurusEngine\ParticleEmitterInstance.h"


CreditsState::CreditsState(StateStack& aStateStack, const bool aInGame) 
	: State(aStateStack, eInputMessengerType::eCredits)
	, myInGame(aInGame)
{
}

CreditsState::~CreditsState()
{
	delete mysprite;
	mysprite = nullptr;
}

void CreditsState::Init()
{
}

eStateStatus CreditsState::Update(const CU::Time & aDeltaTime)
{
	if (mysprite->GetPosition().y > 0)
	{
		mysprite->SetPosition(CU::Vector2f(mysprite->GetPosition().x, mysprite->GetPosition().y + -0.2f * aDeltaTime.GetSeconds()));
	}
	else
	{
		mysprite->SetPosition(CU::Vector2f::Zero);
	}

	return myStatus;
}

void CreditsState::Render()
{
	mysprite->Render();
}

void CreditsState::OnEnter(const bool /*aLetThroughRender*/)
{
	mysprite = new CSpriteInstance("Sprites/Credits/CreditsScreen.dds",CU::Vector2f(1, 1),CU::Vector2f(0,1));
}

void CreditsState::OnExit(const bool /*aLetThroughRender*/)
{
}

void CreditsState::GoToMainMenu()
{
	PostMaster::GetInstance().SendLetter(Message(eMessageType::eStateStackMessage, PopCurrentState()));
}

eMessageReturn CreditsState::Recieve(const Message & aMessage)
{
	return aMessage.myEvent.DoEvent(this);
}

#include "stdafx.h"
#include "CreditsState.h"
#include "PostMaster\PostMaster.h"
#include "PostMaster\PopCurrentState.h"
#include "BrontosaurusEngine\ModelInstance.h"
#include "BrontosaurusEngine\SpriteInstance.h"
#include "CommonUtilities\CUTime.h"
#include "BrontosaurusEngine\ParticleEmitterInstance.h"
#include "../PostMaster/Message.h"
#include "PostMaster/Pop2States.h"

CreditsState::CreditsState(StateStack& aStateStack, const bool aInGame) 
: State(aStateStack),
myInGame(aInGame)
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

void CreditsState::OnEnter(const bool aLetThroughRender)
{
	//PostMaster::GetInstance().Subscribe(this, eMessageType::eStateMessage);
	PostMaster::GetInstance().Subscribe(this, eMessageType::eKeyboardMessage);
	mysprite = new CSpriteInstance("Sprites/Credits/CreditsScreen.dds",CU::Vector2f(1, 1),CU::Vector2f(0,1));
}

void CreditsState::OnExit(const bool /*aLetThroughRender*/)
{
	//PostMaster::GetInstance().UnSubscribe(this, eMessageType::eStateMessage);
	PostMaster::GetInstance().UnSubscribe(this, eMessageType::eKeyboardMessage);
}

void CreditsState::GoToMainMenu()
{
	if (myInGame)
	{
		PostMaster::GetInstance().SendLetter(Message(eMessageType::eStateStackMessage, Pop2States()));
	}
	else
	{
		PostMaster::GetInstance().SendLetter(Message(eMessageType::eStateStackMessage, PopCurrentState()));
	}
}

eMessageReturn CreditsState::Recieve(const Message & aMessage)
{
	//if (aMessage.myMessageType == eMessageType::eStateMessage)
	//{
	//	//TODO::CARL FIX THIST SHIT THING POSTMASTER 2H1t
	//	return aMessage.myEvent.DoEvent(static_cast<State*>(this));
	//}

	return aMessage.myEvent.DoEvent(this);
}

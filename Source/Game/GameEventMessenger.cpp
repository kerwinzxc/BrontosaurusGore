#include "stdafx.h"
#include "GameEventMessenger.h"
#include "PostMaster/Message.h"
#include "PostMaster/Event.h"
#include "PostMaster/PostMaster.h"


CGameEventMessenger::CGameEventMessenger() : myInTweener(nullptr), myOutTweener(nullptr), myCurrentTime(0), myWaitTime(2.f)
{
}

CGameEventMessenger::~CGameEventMessenger()
{
	delete myInTweener, myOutTweener;
	PostMaster::GetInstance().UnSubscribe(this, eMessageType::eGameEventMessage);
}

eMessageReturn CGameEventMessenger::Recieve(const Message& aMessage)
{
	return aMessage.myEvent.DoEvent(this);
}

void CGameEventMessenger::Init(const CU::Vector2f& aPosition)
{
	myText.Init();
	myText.SetPosition(aPosition);
	myText.SetAlignment(eAlignment::CENTER);
	PostMaster::GetInstance().Subscribe(this, eMessageType::eGameEventMessage);
}

void CGameEventMessenger::Update(const float aDeltaTime)
{
	if (myInTweener != nullptr)
	{
		CU::Vector4f colour = myText.GetColor();

		if (myInTweener->IsFinished() == false)
		{
			myInTweener->Update((aDeltaTime));
			colour.a = myInTweener->GetValue();
		}
		else if (myCurrentTime < myWaitTime)
		{
			myCurrentTime += aDeltaTime;
		}
		else if (myOutTweener->IsFinished() == false)
		{
			myOutTweener->Update(aDeltaTime);
			colour.a = myOutTweener->GetValue();
		}
		else if (myTextQueue.Size() > 0)
		{
			SetMessage(myTextQueue.Pop());
		}

		myText.SetColor(colour);
	}
	else if (myTextQueue.Size() > 0)
	{
		SetMessage(myTextQueue.Pop());
	}
}

void CGameEventMessenger::Render()
{
	if (myOutTweener != nullptr && myOutTweener->IsFinished() != true)
	{
		myText.Render();
	}
}

void CGameEventMessenger::AddMessage(CU::GrowingArray<CU::DynamicString> someText)
{
	myTextQueue.Push(someText);
}

void CGameEventMessenger::SetMessage(CU::GrowingArray<CU::DynamicString> someStrings)
{
	if (myInTweener != nullptr)
	{
		delete myInTweener;
		myInTweener = nullptr;
	}
	if (myOutTweener != nullptr)
	{
		delete myOutTweener;
		myOutTweener = nullptr;
	}

	myInTweener = new CU::Tween(CU::TweenType::Quadratic, CU::TweenMod::EaseInOut, 0.f, 1.f, 2.f);
	myOutTweener = new CU::Tween(CU::TweenType::Quadratic, CU::TweenMod::EaseInOut, 1.f, 0.f, 2.f);
	myCurrentTime = 0;

	myText.SetTextLines(someStrings);
}

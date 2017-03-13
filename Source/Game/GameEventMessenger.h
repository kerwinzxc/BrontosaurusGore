#pragma once
#include "../BrontosaurusEngine/TextInstance.h"
#include "Tween.h"
#include "Queue.h"
#include "../ThreadedPostmaster/Subscriber.h"

class CGameEventMessenger :public Postmaster::ISubscriber
{
public:
	CGameEventMessenger();
	~CGameEventMessenger();


	void Init(const CU::Vector2f &aPosition);
	void Update(const float aDeltaTime);
	void Render();

	void AddMessage(CU::GrowingArray<std::wstring>someText);

	eMessageReturn DoEvent(const CGameEventMessage & aGameEventMessageMessage) override;
private:
	void SetMessage(CU::GrowingArray<std::wstring>someStrings);

	CU::Tween* myInTweener;
	CU::Tween* myOutTweener;

	float myCurrentTime;
	const float myWaitTime;

	CTextInstance myText;

	CU::Queue<CU::GrowingArray<std::wstring>> myTextQueue;
};


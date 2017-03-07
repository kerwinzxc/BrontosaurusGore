#pragma once
#include "../BrontosaurusEngine/TextInstance.h"
#include "../PostMaster/Subscriber.h"
#include "Tween.h"
#include "Queue.h"
#include "ThreadedPostmaster/Subscriber.h"

class CGameEventMessenger :public Postmaster::ISubscriber
{
public:
	CGameEventMessenger();
	~CGameEventMessenger();


	void Init(const CU::Vector2f &aPosition);
	void Update(const float aDeltaTime);
	void Render();

	void AddMessage(CU::GrowingArray<std::string>someText);
private:
	void SetMessage(CU::GrowingArray<std::string>someStrings);

	CU::Tween* myInTweener;
	CU::Tween* myOutTweener;

	float myCurrentTime;
	const float myWaitTime;

	CTextInstance myText;

	CU::Queue<CU::GrowingArray<std::string>> myTextQueue;
};


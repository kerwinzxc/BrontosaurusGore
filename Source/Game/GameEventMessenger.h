#pragma once
#include "../BrontosaurusEngine/TextInstance.h"
#include "../PostMaster/Subscriber.h"
#include "Tween.h"
#include "Queue.h"

class CGameEventMessenger :public Subscriber
{
public:
	CGameEventMessenger();
	~CGameEventMessenger();

	eMessageReturn Recieve(const Message& aMessage) override;

	void Init(const CU::Vector2f &aPosition);
	void Update(const float aDeltaTime);
	void Render();

	void AddMessage(CU::GrowingArray<CU::DynamicString>someText);
private:
	void SetMessage(CU::GrowingArray<CU::DynamicString>someStrings);

	CU::Tween* myInTweener;
	CU::Tween* myOutTweener;

	float myCurrentTime;
	const float myWaitTime;

	CTextInstance myText;

	CU::Queue<CU::GrowingArray<CU::DynamicString>> myTextQueue;
};


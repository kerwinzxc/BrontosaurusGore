#pragma once
#include "Component.h"
#include <queue>
#include "..\BrontosaurusEngine\SColorGradeFade.h"

enum ELUTType : char;

class CLutComponent : public CComponent
{
public:
	CLutComponent();
	~CLutComponent();

	virtual void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
	void Update(const float aDeltaTime);

private:
	void PushFade(const ELUTType aFadeTo, const float aTime, const bool aInterrupt);
	void PopFade();

private:
	float myFadeTimer;
	SColorFade myDefaultFade;
	std::queue<SColorFade> myFadeQueue;
};

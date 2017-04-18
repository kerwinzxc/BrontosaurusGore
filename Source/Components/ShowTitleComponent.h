#pragma once
#include "Component.h"

class CShowTitleComponent :	public CComponent
{
public:
	CShowTitleComponent();
	~CShowTitleComponent();

	void Update(const float aDT);
	const float GetAlpha() { return myAlpha; };
	const int GetIndex() { return myCurrentIndex; };

	void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData) override;
private:
	float myAlpha;
	bool myFadeIn;
	int myCurrentIndex;
};


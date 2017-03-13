#include "stdafx.h"
#include "CheckPointComponent.h"
#include "../ThreadedPostmaster/Postmaster.h"

CCheckPointComponent::CCheckPointComponent()
{
}


CCheckPointComponent::~CCheckPointComponent()
{
}

void CCheckPointComponent::Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData)
{
	aMessageData;
	switch (aMessageType)
	{
	default:
		break;
	}
}
void CCheckPointComponent::Destroy()
{

}
void CCheckPointComponent::Update(float aDeltaTime)
{
	aDeltaTime;
}

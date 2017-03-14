#include "stdafx.h"
#include "PollingStation.h"

CPollingStation* CPollingStation::ourInstance = nullptr;

CPollingStation::CPollingStation()
	: myPlayerObject(nullptr)
{
	assert(ourInstance == nullptr);
	ourInstance = this;
}

CPollingStation::~CPollingStation()
{
	assert(ourInstance == this);
	ourInstance = nullptr;
}

CPollingStation* CPollingStation::GetInstance()
{
	return ourInstance;
}

void CPollingStation::SetPlayerObject(const CGameObject* aPlayerObject)
{
	myPlayerObject = aPlayerObject;
}

const CGameObject* CPollingStation::GetPlayerObject()
{
	return myPlayerObject;
}

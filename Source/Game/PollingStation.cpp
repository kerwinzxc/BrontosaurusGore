#include "stdafx.h"
#include "PollingStation.h"

CPollingStation* CPollingStation::ourInstance = nullptr;

CPollingStation::CPollingStation()
	: myPlayerObject(nullptr)
{
	assert(ourInstance == nullptr);
	ourInstance = this;
	myKeys.Init(1);
	myPLayers.Init(2);
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

void CPollingStation::SetPlayerObject(CGameObject* aPlayerObject)
{
	myPlayerObject = aPlayerObject;
}

CGameObject* CPollingStation::GetPlayerObject()
{
	return myPlayerObject;
}

void CPollingStation::AddKey(const short aKeyId)
{
	myKeys.Add(aKeyId);
}

void CPollingStation::ClearKeys()
{
	myKeys.RemoveAll();
}

const CU::GrowingArray<short>& CPollingStation::GetKeys() const
{
	return myKeys;
}

void CPollingStation::AddPlayerObject(CGameObject* aPLayerObject)
{
	myPLayers.Add(aPLayerObject);
}
const CU::GrowingArray<CGameObject*>& CPollingStation::GetPlayers() const
{
	return myPLayers;
}
const unsigned int CPollingStation::GetNumberOfPlayers() const
{
	return myPLayers.Size();
}

#include "stdafx.h"
#include "PollingStation.h"

CPollingStation* CPollingStation::ourInstance = nullptr;

CPollingStation::CPollingStation()
	: myPlayerObject(nullptr)
{
	assert(ourInstance == nullptr);
	ourInstance = this;
	myCurrentLevelIndex = 1337;
	myKeys.Init(1);
	myPLayers.Init(2);
	myUnlockedWeaponIDs.Init(4);
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

void  CPollingStation::AddWeapon(const unsigned short aWeaponID)
{
	myUnlockedWeaponIDs.Add(aWeaponID);
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

bool CPollingStation::CheckIfIsPlayerObject(CGameObject* aGameObjectToCheck)
{
	if(myPLayers.Find(aGameObjectToCheck) == CU::GrowingArray<CGameObject*>::FoundNone)
	{
		return false;
	}
	return true;
}

void CPollingStation::SetCurrentLevelIndex(unsigned char aLevelIndex)
{
	myCurrentLevelIndex = aLevelIndex;
}

unsigned char CPollingStation::GetCurrentLevelIndex() const
{
	return myCurrentLevelIndex;
}
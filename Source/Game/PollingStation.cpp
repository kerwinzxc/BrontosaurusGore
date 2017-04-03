#include "stdafx.h"
#include "PollingStation.h"

CPollingStation* CPollingStation::ourInstance = nullptr;

CPollingStation::CPollingStation()
	: myPlayerObject(nullptr)
{
	assert(ourInstance == nullptr);
	ourInstance = this;
	myKeys.Init(1);
	myWeaponIndexes.Init(4);
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

void CPollingStation::AddWeapon(const int aWeaponId)
{
	myWeaponIndexes.Add(aWeaponId);
}

void CPollingStation::RemoveWeapon(const int aWeaponId)
{
	myWeaponIndexes.Remove(aWeaponId);
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

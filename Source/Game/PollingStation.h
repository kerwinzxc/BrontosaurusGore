#pragma once

class CGameObject;

class CPollingStation
{
public:
	CPollingStation();
	~CPollingStation();

	static CPollingStation* GetInstance();

	void SetPlayerObject(CGameObject* aPlayerObject);
	CGameObject* GetPlayerObject();

	void AddWeapon(const unsigned short aWeaponID);

	void AddKey(const short aKeyId);
	void ClearKeys();
	const CU::GrowingArray<short>& GetKeys() const;
private:
	CGameObject* myPlayerObject;

	CU::GrowingArray<unsigned short> myUnlockedWeaponIDs;
	CU::GrowingArray<short> myKeys;

	static CPollingStation* ourInstance;
};

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

	void AddWeapon(const int aWeaponId);
	void RemoveWeapon(const int aWeaponId);

	void AddKey(const short aKeyId);
	void ClearKeys();
	const CU::GrowingArray<short>& GetKeys() const;
private:
	CGameObject* myPlayerObject;
	CU::GrowingArray<int> myWeaponIndexes;
	CU::GrowingArray<short> myKeys;

	static CPollingStation* ourInstance;
};

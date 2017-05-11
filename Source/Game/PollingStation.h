#pragma once

class CGameObject;

class CPollingStation
{
public:
	CPollingStation();
	~CPollingStation();
	static bool CheckInstance();

	static CPollingStation* GetInstance();

	void SetPlayerObject(CGameObject* aPlayerObject);
	CGameObject* GetPlayerObject();
	void AddPlayerObject(CGameObject* aPLayerObject);
	const CU::GrowingArray<CGameObject*>&GetPlayers() const;
	const unsigned int GetNumberOfPlayers() const;

	void AddWeapon(const unsigned short aWeaponID);

	void AddKey(const short aKeyId);
	void ClearKeys();
	const CU::GrowingArray<short>& GetKeys() const;

	bool CheckIfIsPlayerObject(CGameObject* aGameObjectToCheck);

	void SetCurrentLevelIndex(unsigned char aLevelIndex);
	unsigned char GetCurrentLevelIndex() const;

private:
	unsigned short myCurrentLevelIndex;
	CGameObject* myPlayerObject;
	CU::GrowingArray<CGameObject*> myPLayers;

	CU::GrowingArray<unsigned short> myUnlockedWeaponIDs;
	CU::GrowingArray<short> myKeys;

	static CPollingStation* ourInstance;
};

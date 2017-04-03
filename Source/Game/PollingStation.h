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
	void AddPlayerObject(CGameObject* aPLayerObject);
	const CU::GrowingArray<CGameObject*>&GetPlayers() const;
	const unsigned int GetNumberOfPlayers() const;

	void AddKey(const short aKeyId);
	void ClearKeys();
	const CU::GrowingArray<short>& GetKeys() const;
private:
	CGameObject* myPlayerObject;
	CU::GrowingArray<CGameObject*> myPLayers;
	CU::GrowingArray<short> myKeys;

	static CPollingStation* ourInstance;
};

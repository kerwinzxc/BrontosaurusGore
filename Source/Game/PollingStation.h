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

	void AddKey(const short aKeyId);
	void ClearKeys();
	const CU::GrowingArray<short>& GetKeys() const;
private:
	CGameObject* myPlayerObject;
	CU::GrowingArray<short> myKeys;

	static CPollingStation* ourInstance;
};

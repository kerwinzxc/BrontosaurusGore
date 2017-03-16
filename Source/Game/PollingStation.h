#pragma once

class CGameObject;

class CPollingStation
{
public:
	CPollingStation();
	~CPollingStation();

	static CPollingStation* GetInstance();

	void SetPlayerObject(const CGameObject* aPlayerObject);
	const CGameObject* GetPlayerObject();

private:
	const CGameObject* myPlayerObject;

	static CPollingStation* ourInstance;
};

#pragma once

class ISettingsSubscriber
{
public:
	ISettingsSubscriber();
	~ISettingsSubscriber();

	virtual void UpdateSettings() = 0;
};

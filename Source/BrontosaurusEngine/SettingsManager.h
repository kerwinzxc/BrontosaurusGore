#pragma once

#include "../CommonUtilities/BitSet.h"

class CommandLineManager;
class ISettingsSubscriber;

class CSettingsManager
{
public:
	enum class eSetting : unsigned int
	{
		eFullsScreen,
		eBloom,
		eHDR,
		eMSAA,
		eMotionblur,
		eCromaticAberration,
		eLength,
	};

	static const unsigned int SettingsCount = static_cast<unsigned int>(eSetting::eLength);

	CSettingsManager();
	~CSettingsManager();

	void Init(CommandLineManager& aCommandLineManager);
	void UpdateSettings();

	bool GetSetting(const eSetting aSetting) const;
	void ChangeSetting(const eSetting aSetting, const bool aNewValue);

private:
	CU::CBitSet<SettingsCount> myBools;

	CU::GrowingArray<ISettingsSubscriber*> mySubscribers;
};


#include "stdafx.h"
#include "SettingsManager.h"
#include "SettingsSubscriber.h"

#include "../CommonUtilities/CommandLineManager.h"

CSettingsManager::CSettingsManager()
{
}

CSettingsManager::~CSettingsManager()
{
}

void CSettingsManager::Init(CommandLineManager& /*aCommandLineManager*/)
{
	//if (!aCommandLineManager.HasParameter("-windowed"))
	//{
	//	myBools[SINDEX(eFullsScreen)] = true;
	//}
	//if (!aCommandLineManager.HasParameter("-nobloom"))
	//{
	//	myBools[eSetting::eBloom] = true;
	//}
	//if (!aCommandLineManager.HasParameter("-nohdr"))
	//{
	//	myBools[eSetting::eHDR] = true;
	//}
	//if (!aCommandLineManager.HasParameter("-noMSAA"))
	//{
	//	myBools[eSetting::eMSAA] = true;
	//}
	//if (!aCommandLineManager.HasParameter("-nomotionblur"))
	//{
	//	myBools[eSetting::eMotionblur] = true;
	//}
	//if (!aCommandLineManager.HasParameter("-nochromaticaberration"))
	//{
	//	myBools[eSetting::eCromaticAberration] = true;
	//}

	UpdateSettings();
}

void CSettingsManager::UpdateSettings()
{
	for (ISettingsSubscriber* subscriber : mySubscribers)
	{
		subscriber->UpdateSettings();
	}
}

bool CSettingsManager::GetSetting(const eSetting /*aSetting*/) const
{
	//return myBools[aSetting];
	return false;
}

void CSettingsManager::ChangeSetting(const eSetting /*aSetting*/, const bool /*aNewValue*/)
{
	//myBools[aSetting] = aNewValue;
}

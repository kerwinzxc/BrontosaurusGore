#include "stdafx.h"
#include "LoadSoundComponent.h"
#include "KevinLoader/KevinLoader.h"
#include "LoadManager.h"
#include "ComponentManager.h"
#include "AudioSourceComponent.h"
#include "AudioSourceComponentManager.h"

int LoadSoundComponent(KLoader::SLoadedComponentData someData)
{
	CAudioSourceComponent* audio = CAudioSourceComponentManager::GetInstance().CreateComponent();
	
	return audio->GetId();
}

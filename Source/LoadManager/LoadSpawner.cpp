#include "stdafx.h"
#include "LoadSpawner.h"
#include "../Components/SpawnerManager.h"
#include "../Components/Spawner.h"

int LoadSpawnerComponent(KLoader::SLoadedComponentData someData)
{

	GET_SERVERLOADMANAGER(loadManager);

	CU::CJsonValue list = someData.myData.at("WaveList"); //h�mta ut denna arrayen

	CU::GrowingArray<unsigned char> waveList;
	waveList.Init(list.Size(), true);

	for (unsigned char i = 0; i <list.Size(); ++i)
	{
		waveList.Add(list[i].GetUchar());
	}

	//loopa igenom den arrayen
	//skapa en wave lista f�r varje element
	//h�mta ut varje array i den arrayen
	//loopa igenom dem arrayerna
	//fyll varje wave lista med fiender fr�n dessa arrayer

	CSpawnerComponent* spawner = loadManager.GetCurrentGameServer().GetSpawnerManager()->CreateSpawnerComponent(waveList, someData.myData.at("EnemyType").GetEnum<eEnemyTypes>(), loadManager.GetCurrentGameServer().GetThreadID());

	return spawner->GetId();
}

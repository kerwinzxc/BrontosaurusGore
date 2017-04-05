#include "stdafx.h"
#include "LoadSpawner.h"
#include "../Components/SpawnerManager.h"
#include "../Components/Spawner.h"

int LoadSpawnerComponent(KLoader::SLoadedComponentData someData)
{

	GET_SERVERLOADMANAGER(loadManager);
	CU::CJsonValue list;
	CU::GrowingArray<unsigned char> waveList;
	if (someData.myData.Count("WaveList") > 0)
	{
		list = someData.myData.at("WaveList"); //hämta ut denna arrayen
		waveList.Init(list.Size(), true);

		for (unsigned char i = 0; i <list.Size(); ++i)
		{
			waveList.Add(list[i].GetUchar());
		}
	}
	else
	{
		DL_MESSAGE_BOX("Fattas Lista i Spawner component leta i leveldata");
		waveList.Init(1, true);
	}


	//loopa igenom den arrayen
	//skapa en wave lista för varje element
	//hämta ut varje array i den arrayen
	//loopa igenom dem arrayerna
	//fyll varje wave lista med fiender från dessa arrayer

	CSpawnerComponent* spawner = loadManager.GetCurrentGameServer().GetSpawnerManager()->CreateSpawnerComponent(waveList, someData.myData.at("EnemyType").GetEnum<eEnemyTypes>(), loadManager.GetCurrentGameServer().GetThreadID());

	return spawner->GetId();
}

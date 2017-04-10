#include "stdafx.h"
#include "LoadSpawner.h"
#include "../Components/SpawnerManager.h"
#include "../Components/Spawner.h"

int LoadSpawnerComponent(KLoader::SLoadedComponentData someData)
{

	GET_SERVERLOADMANAGER(loadManager);
	CU::CJsonValue list;
	CU::GrowingArray<unsigned short> waveList;
	if (someData.myData.Count("WaveList") > 0)
	{
		list = someData.myData.at("WaveList"); //h�mta ut denna arrayen
		waveList.Init(list.Size(), true);
		waveList.RemoveAll();
		//unsigned short it;
		//unsigned short what;
		//unsigned short size = 0;
		for (unsigned char i = 0; i <list.Size(); ++i)
		{
			//size++;
			//it = list[i].GetUInt();
			waveList.Add(list[i].GetUInt());
			//what = waveList.GetLast();
		}
	}
	else
	{
		DL_MESSAGE_BOX("Fattas Lista i Spawner component kolla s� att v�rdena �r r�tt i unity scenen");
		waveList.Init(1, true);
	}


	//loopa igenom den arrayen
	//skapa en wave lista f�r varje element
	//h�mta ut varje array i den arrayen
	//loopa igenom dem arrayerna
	//fyll varje wave lista med fiender fr�n dessa arrayer

	CSpawnerComponent* spawner = loadManager.GetCurrentGameServer().GetSpawnerManager()->CreateSpawnerComponent(waveList, someData.myData.at("EnemyType").GetEnum<eEnemyTypes>(), loadManager.GetCurrentGameServer().GetThreadID());

	return spawner->GetId();
}

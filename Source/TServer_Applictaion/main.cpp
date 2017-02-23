// TServer_Applictation.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../TServer/GameServer.h"
#include "../CommonUtilities/DL_Debug.h"


int main(int argc, char* argv[])
{
	//CommandLineManager::CreateInstance(argc, argv);
	DL_Debug::Debug::CreateInstance();
	//PostMaster::CreateInstance();

	CGameServer gameServer;
	gameServer.Init();
	gameServer.Start();

	//CommandLineManager::DestroyInstance();
	DL_Debug::Debug::DestroyInstance();
	//PostMaster::DestroyInstance();

    return 0;
}


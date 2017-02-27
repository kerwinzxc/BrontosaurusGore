// TServer_Applictation.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../CommonUtilities/DL_Debug.h"
#include "../TServer/ServerMain.h"


int main(int argc, char* argv[])
{
	//CommandLineManager::CreateInstance(argc, argv);
	DL_Debug::Debug::CreateInstance();
	//PostMaster::CreateInstance();

	CServerMain server;
	server.StartServer();

	//CommandLineManager::DestroyInstance();
	DL_Debug::Debug::DestroyInstance();
	//PostMaster::DestroyInstance();

    return 0;
}


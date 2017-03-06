// TServer_Applictation.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../CommonUtilities/DL_Debug.h"
#include "../TServer/ServerMain.h"
#include "../ThreadedPostmaster/Postmaster.h"


int main(int argc, char* argv[])
{
	Postmaster::Threaded::CPostmaster::Create();
	//CommandLineManager::CreateInstance(argc, argv);
	DL_Debug::Debug::CreateInstance();
	//PostMaster::CreateInstance();

	CServerMain server;
	server.StartServer();

	//CommandLineManager::DestroyInstance();
	DL_Debug::Debug::DestroyInstance();
	//PostMaster::DestroyInstance();
	Postmaster::Threaded::CPostmaster::Destroy();

    return 0;
}


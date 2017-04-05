// TServer_Applictation.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../CommonUtilities/DL_Debug.h"
#include "../TServer/ServerMain.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../CommonUtilities/ConsolePrinter.h"
CServerMain server;

BOOL WINAPI HandleShutdown(DWORD type)
{
	switch (type)
	{
	case CTRL_CLOSE_EVENT:
	case CTRL_SHUTDOWN_EVENT:
		server.Shutdown();
		while(server.IsClosed() == false)
		{
			
		}
	{
			int i = 0;
	}
		break;
	default:
		return FALSE;
	}
	return TRUE;
}

int main(int argc, char* argv[])
{
	Postmaster::Threaded::CPostmaster::Create();
	//CommandLineManager::CreateInstance(argc, argv);
	DL_Debug::Debug::CreateInstance();
	//PostMaster::CreateInstance();
	
	if (SetConsoleCtrlHandler(HandleShutdown, true) == false)
	{
		DL_PRINT(std::string("Failed to create console handler.").c_str());
	}

	server.StartServer();

	//CommandLineManager::DestroyInstance();
	DL_Debug::Debug::DestroyInstance();
	//PostMaster::DestroyInstance();
	Postmaster::Threaded::CPostmaster::Destroy();

    return 0;
}


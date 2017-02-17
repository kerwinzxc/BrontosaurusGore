#include "stdafx.h"
#include "TServer_NetworkWrapper.h"
#include <minwinbase.h>


TServer_NetworkWrapper::TServer_NetworkWrapper() : myListenSocket(INVALID_SOCKET)
{
}


TServer_NetworkWrapper::~TServer_NetworkWrapper()
{

}

bool TServer_NetworkWrapper::Init(const int aPortNumber)
{	
	return false;
}

bool TServer_NetworkWrapper::Listen()
{
	return false;
}

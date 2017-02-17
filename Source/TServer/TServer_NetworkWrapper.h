#pragma once
#include "TShared_NetworkWrapper.h"

class TServer_NetworkWrapper: public TShared_NetworkWrapper
{
public:
	TServer_NetworkWrapper();
	~TServer_NetworkWrapper();

	bool Init(const int aPortNumber);
	bool Listen();
private:
	SOCKET myListenSocket;
};


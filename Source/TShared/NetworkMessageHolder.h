#pragma once
#include <vector>
#include "../TServer/TServer_NetworkWrapper.h"

struct SNetworkMessageHolder
{
	SNetworkPackageHeader myHeader;
	std::vector<char> Stream;
};


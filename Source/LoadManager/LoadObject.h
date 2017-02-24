#pragma once

namespace KLoader
{
	struct SLoadedComponentData;
}

int LoadObject(KLoader::SLoadedComponentData someData);
int LoadServerObject(KLoader::SLoadedComponentData someData);
void LinkObject(const int target,const int otherComponent);
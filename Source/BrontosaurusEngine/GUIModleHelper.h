#pragma once

class CEffect;
class CSurface;
class CLoaderMesh;

namespace GUIModleHelper
{
	CEffect* CreateEffect(const CLoaderMesh* aLoaderMesh);
	CSurface* CreateSurface(const char* aTexturePath);
	CSurface* CreateSurface(const CU::GrowingArray<std::string>& aTexturePaths);
}

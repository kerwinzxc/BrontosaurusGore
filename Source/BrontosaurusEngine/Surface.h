#pragma once
//#include <GrowingArray.h>

class CDXFramework;
class CTexture;

class CSurface
{
	enum TextureIndex
	{
		eDiffuse = 0,
		eRoughness = 1,
		eAmbientOcclusion,
		eEmissive,
		eNormal = 5, 
		eMetalness = 10
	};


public:
	CSurface(const std::string & aDirectory, const CU::GrowingArray<std::string>& aTexturePathList);
	CSurface(const CU::GrowingArray<const wchar_t*>& aTexturePathList);
	CSurface(const CSurface& aSurface);
	~CSurface();
	void Activate();

	const CU::Vector2ui& GetTextureSize() const;
	CU::Vector2f GetTextureSizeFloat() const;

private:
	CU::GrowingArray<CTexture*> myTextures;
	CDXFramework* myFramework;
};


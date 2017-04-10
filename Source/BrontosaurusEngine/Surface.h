#pragma once
//#include <GrowingArray.h>

class CDXFramework;
class CTexture;

class CSurface
{
public:
	enum TextureIndex
	{
		eDiffuse = 0,
		eRoughness = 1,
		eAmbientOcclusion,
		eEmissive,
		eNormal = 4,
		eNormal2 = 5,
		eMetalness = 10
	};

	CSurface(const std::string & aDirectory, const CU::GrowingArray<std::string>& aTexturePathList);
	CSurface(const CU::GrowingArray<const wchar_t*>& aTexturePathList);
	CSurface(const CSurface& aSurface);
	~CSurface();

	CSurface& operator=(const CSurface& aSurface);

	void Activate();

	const CU::Vector2ui& GetTextureSize() const;
	CU::Vector2f GetTextureSizeFloat() const;

private:
	CU::GrowingArray<CTexture*> myTextures;
	CDXFramework* myFramework;
};


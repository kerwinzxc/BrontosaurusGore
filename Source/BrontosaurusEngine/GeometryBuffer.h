#pragma once
#include "RenderPackage.h"

class CDXFramework;

class CGeometryBuffer
{
public:
	friend class CDeferredRenderer;

	using EGeometryPackages = unsigned char;
#define NUM_GB_PACKAGES 4u
	enum EGeometryPackage : EGeometryPackages
	{
		eDiffuse	= (1 << 0),
		eNormal		= (1 << 1),
		eRMAO		= (1 << 2),
		eEmissive	= (1 << 3),
		eDepth		= (1 << 4),
		eALL		= (1 << 5) - 1
	};
	


public:
	CGeometryBuffer();
	~CGeometryBuffer();

	void Init(const CU::Vector2ui & aSize, CDXFramework* aFramework, ID3D11Texture2D * aTexture = nullptr, DXGI_FORMAT aFormat = DXGI_FORMAT_R8G8B8A8_UNORM);
	void BindInput(const EGeometryPackages aMask = EGeometryPackage::eALL, const EGeometryPackage aDepthPackage = EGeometryPackage::eDiffuse);
	void BindOutput(const EGeometryPackages aMask = EGeometryPackage::eALL, const EGeometryPackage aDepthPackage = EGeometryPackage::eDiffuse);

	void UnbindInput();
	void UnbindOutput();
	void Clear();

	CRenderPackage& GetRenderPackage(const EGeometryPackage aPackageType);

	bool IsInited();

private:
	CU::StaticArray<CRenderPackage, NUM_GB_PACKAGES> myPackages;
	CDXFramework* myFramework;
};

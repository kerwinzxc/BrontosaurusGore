#pragma once
#include "RenderPackage.h"

class CDXFramework;

class CGeometryBuffer
{
public:
	friend class CDeferredRenderer;

	using EGeometryPackages = unsigned char;
	enum EGeometryPackage : EGeometryPackages
	{
		eDiffuse	= (1 << 0),
		eNormal		= (1 << 1),
		eRMAO		= (1 << 2),
		eEmissive	= (1 << 3),
		eDepth		= (1 << 4),
		eHighlight	= (1 << 5),

		eALL		= (1 << 6) - 1
	};

public:
	CGeometryBuffer();
	~CGeometryBuffer();

	void Init(const CU::Vector2ui & aSize, CDXFramework* aFramework, ID3D11Texture2D * aTexture = nullptr, DXGI_FORMAT aFormat = DXGI_FORMAT_R8G8B8A8_UNORM);
	void BindInput(const EGeometryPackages aMask = EGeometryPackage::eALL);
	void BindOutput(const EGeometryPackages aMask = EGeometryPackage::eALL);
	
	void UnbindInput();
	void UnbindOutput();
	void Clear();

private:
	CRenderPackage myDiffuse;
	CRenderPackage myNormal;
	CRenderPackage myRMAO;
	CRenderPackage myEmissive;
	CRenderPackage myHighLight;
	CDXFramework* myFramework;
};

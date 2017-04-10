#include "stdafx.h"
#include "GeometryBuffer.h"
#include "DXFramework.h"

CGeometryBuffer::CGeometryBuffer()
{
	myFramework = nullptr;
}

CGeometryBuffer::~CGeometryBuffer()
{
	myFramework = nullptr;
}

void CGeometryBuffer::Init(const CU::Vector2ui & aSize, CDXFramework* aFramework, ID3D11Texture2D * aTexture /*= nullptr*/, DXGI_FORMAT aFormat /*= DXGI_FORMAT_R8G8B8A8_UNORM*/)
{
	if (aFramework == nullptr)
	{
		DL_ASSERT("aFramework was null");
	}
	myFramework = aFramework;

	myPackages[0].Init(aSize, aTexture, aFormat);
	myPackages[1].Init(aSize, aTexture, aFormat);
	myPackages[2].Init(aSize, aTexture, aFormat);
	myPackages[3].Init(aSize, aTexture, aFormat);
}

void CGeometryBuffer::BindInput(const EGeometryPackages aMask, const EGeometryPackage aDepthPackage/* = EGeometryPackage::eDiffuse*/)
{
	ID3D11ShaderResourceView* srvs[6];
	srvs[0] = (aMask & EGeometryPackage::eDiffuse)	? myPackages[0].GetResource()	: nullptr;
	srvs[1] = (aMask & EGeometryPackage::eNormal)	? myPackages[1].GetResource()	: nullptr;
	srvs[2] = (aMask & EGeometryPackage::eRMAO)		? myPackages[2].GetResource()	: nullptr;
	srvs[3] = (aMask & EGeometryPackage::eEmissive) ? myPackages[3].GetResource()	: nullptr;
	srvs[4] = (aMask & EGeometryPackage::eDepth)	? GetRenderPackage(aDepthPackage).GetDepthResource() : nullptr;
	srvs[5] = (aMask & EGeometryPackage::eDepth)	? GetRenderPackage(eEmissive).GetDepthResource()	: nullptr;
	myFramework->GetDeviceContext()->PSSetShaderResources(1, 6, srvs);
}

void CGeometryBuffer::BindOutput(const EGeometryPackages aMask, const EGeometryPackage aDepthPackage/* = EGeometryPackage::eDiffuse*/)
{
	ID3D11RenderTargetView* rtvs[5];
	rtvs[0] = (aMask & EGeometryPackage::eDiffuse)		? myPackages[0].GetRenderTargetView() : nullptr;
	rtvs[1] = (aMask & EGeometryPackage::eNormal)		? myPackages[1].GetRenderTargetView() : nullptr;
	rtvs[2] = (aMask & EGeometryPackage::eRMAO)			? myPackages[2].GetRenderTargetView() : nullptr;
	rtvs[3] = (aMask & EGeometryPackage::eEmissive)		? myPackages[3].GetRenderTargetView() : nullptr;
	myFramework->GetDeviceContext()->OMSetRenderTargets(4, rtvs, (aMask & EGeometryPackage::eDepth) ? GetRenderPackage(aDepthPackage).GetDepthStencilView() : nullptr);
}

void CGeometryBuffer::UnbindInput()
{
	ID3D11ShaderResourceView* srvs[5];
	srvs[0] = nullptr;
	srvs[1] = nullptr;
	srvs[2] = nullptr;
	srvs[3] = nullptr;
	srvs[4] = nullptr;
	myFramework->GetDeviceContext()->PSSetShaderResources(1, 5, srvs);
}

void CGeometryBuffer::UnbindOutput()
{
	ID3D11RenderTargetView* rtvs[6];
	rtvs[0] = nullptr;
	rtvs[1] = nullptr;
	rtvs[2] = nullptr;
	rtvs[3] = nullptr;
	rtvs[4] = nullptr;
	rtvs[5] = nullptr;
	myFramework->GetDeviceContext()->OMSetRenderTargets(5, rtvs, nullptr);
}

void CGeometryBuffer::Clear()
{
	for (int i = 0; i < NUM_GB_PACKAGES; ++i)
	{
		myPackages[i].Clear();
	}
}

CRenderPackage& CGeometryBuffer::GetRenderPackage(const EGeometryPackage aPackageType)
{
	switch (aPackageType)
	{
	case EGeometryPackage::eDiffuse:
		return myPackages[0];
	case EGeometryPackage::eNormal:
		return myPackages[1];
	case EGeometryPackage::eRMAO:
		return myPackages[2];
	case EGeometryPackage::eEmissive:
		return myPackages[3];
	default:
		break;
	}
}

bool CGeometryBuffer::IsInited()
{
	return myFramework != nullptr;
}


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

	myDiffuse.Init(aSize, aTexture, aFormat);
	myNormal.Init(aSize, aTexture, aFormat);
	myRMAO.Init(aSize, aTexture, aFormat);
	myEmissive.Init(aSize, aTexture, aFormat);
}

void CGeometryBuffer::BindInput(const EGeometryPackages aMask)
{
	ID3D11ShaderResourceView* srvs[5];
	srvs[0] = (aMask & EGeometryPackage::eDiffuse)	? myDiffuse.GetResource() : nullptr;
	srvs[1] = (aMask & EGeometryPackage::eNormal)	? myNormal.GetResource() : nullptr;
	srvs[2] = (aMask & EGeometryPackage::eRMAO)		? myRMAO.GetResource() : nullptr;
	srvs[3] = (aMask & EGeometryPackage::eEmissive) ? myEmissive.GetResource() : nullptr;
	srvs[4] = (aMask & EGeometryPackage::eDepth)	? myDiffuse.GetDepthResource() : nullptr;
	myFramework->GetDeviceContext()->PSSetShaderResources(1, 5, srvs);
}

void CGeometryBuffer::BindOutput(const EGeometryPackages aMask)
{
	ID3D11RenderTargetView* rtvs[5];
	rtvs[0] = (aMask & EGeometryPackage::eDiffuse)		? myDiffuse.GetRenderTargetView() : nullptr;
	rtvs[1] = (aMask & EGeometryPackage::eNormal)		? myNormal.GetRenderTargetView() : nullptr;
	rtvs[2] = (aMask & EGeometryPackage::eRMAO)			? myRMAO.GetRenderTargetView() : nullptr;
	rtvs[3] = (aMask & EGeometryPackage::eEmissive)		? myEmissive.GetRenderTargetView() : nullptr;
	myFramework->GetDeviceContext()->OMSetRenderTargets(4, rtvs, myDiffuse.GetDepthStencilView());
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
	myDiffuse.Clear();
	myNormal.Clear();
	myRMAO.Clear();
	myEmissive.Clear();
}

bool CGeometryBuffer::IsInited()
{
	return myFramework != nullptr;
}

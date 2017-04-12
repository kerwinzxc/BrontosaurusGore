#include "stdafx.h"
#include "RenderPackage.h"
#include "Engine.h"
#include "DXFramework.h"
#include "ScreenGrab/ScreenGrab.h"
#include <vector2.h>
#include <minwinbase.h>

CRenderPackage::CRenderPackage(): myDepthTexture(nullptr), myDepthTarget(nullptr)
{
	myTexture = nullptr;
	myResource = nullptr;
	myTarget = nullptr;
	myDepth = nullptr;
	myViewport = nullptr;
	myDepthResource = nullptr;
}

CRenderPackage::~CRenderPackage()
{
	SAFE_RELEASE(myTexture);
	SAFE_RELEASE(myResource);
	SAFE_RELEASE(myTarget);
	SAFE_RELEASE(myDepthTarget);
	SAFE_RELEASE(myDepth);
	SAFE_RELEASE(myDepthResource);
	SAFE_DELETE(myViewport);
}

void CRenderPackage::Init(const CU::Vector2ui & aSize, ID3D11Texture2D * aTexture, DXGI_FORMAT aFormat)
{
	HRESULT result;
	mySize = aSize;

	myViewport = new D3D11_VIEWPORT;
	myViewport->TopLeftX = 0.f;
	myViewport->TopLeftY = 0.f;
	myViewport->MinDepth = 0.f;
	myViewport->MaxDepth = 1.f;
	myViewport->Width = static_cast<float>(aSize.x);
	myViewport->Height = static_cast<float>(aSize.y);

	myTexture = aTexture;
	if (myTexture == nullptr)
	{
		CreateTexture2D(aSize.x, aSize.y, aFormat);
		result = DEVICE->CreateShaderResourceView(myTexture, nullptr, &myResource);
		CHECK_RESULT(result, "Couldn't create shader resourse view for the RenderPackage");
	}

	result = DEVICE->CreateRenderTargetView(myTexture, NULL, &myTarget);
	CHECK_RESULT(result, "Couldn't create render target view for the RenderPackage");

	FRAMEWORK->CreateDepthStencil(static_cast<unsigned int>(myViewport->Width), static_cast<unsigned int>(myViewport->Height), myDepth, myDepthResource);

}

void CRenderPackage::ReInit(const CU::Vector2ui& aSize, ID3D11Texture2D* aTexture, DXGI_FORMAT aFormat)
{
	SAFE_RELEASE(myTexture);
	SAFE_RELEASE(myResource);
	SAFE_RELEASE(myTarget);
	SAFE_RELEASE(myDepth);
	SAFE_RELEASE(myDepthResource);
	SAFE_DELETE(myViewport);

	Init(aSize, aTexture, aFormat);
}

void CRenderPackage::Clear(const float aDepthValue)
{
	ID3D11DeviceContext* context = DEVICE_CONTEXT;
	float clearColour[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	context->ClearRenderTargetView(myTarget, clearColour);
	context->ClearDepthStencilView(myDepth, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, aDepthValue, 0);
}

void CRenderPackage::Activate()
{
	ID3D11DeviceContext* context = DEVICE_CONTEXT;

	context->OMSetRenderTargets(1, &myTarget, myDepth);
	context->RSSetViewports(1, myViewport);
}

void CRenderPackage::Activate(CRenderPackage& aRenderPackage)
{
	ID3D11DeviceContext* context = DEVICE_CONTEXT;

	D3D11_VIEWPORT viewPorts[2];
	ID3D11RenderTargetView* renderTargets[2];

	viewPorts[0] = *myViewport;
	viewPorts[1] = *aRenderPackage.myViewport;

	renderTargets[0] = myTarget;
	renderTargets[1] = aRenderPackage.myTarget;

	context->OMSetRenderTargets(2, &renderTargets[0], myDepth);
	context->RSSetViewports(2, &viewPorts[0]);
}

void CRenderPackage::ActivateToDepth()
{
	ID3D11DeviceContext* context = DEVICE_CONTEXT;

	context->OMSetRenderTargets(1, &myDepthTarget, myDepth);
	context->RSSetViewports(1, myViewport);
}

ID3D11ShaderResourceView *& CRenderPackage::GetDepthResource()
{
	return myDepthResource;
}

ID3D11ShaderResourceView *& CRenderPackage::GetResource()
{
	return myResource;
}

ID3D11RenderTargetView *& CRenderPackage::GetRenderTargetView()
{
	return myTarget;
}

ID3D11DepthStencilView*& CRenderPackage::GetDepthStencilView()
{
	return myDepth;
}

ID3D11Texture2D *& CRenderPackage::GetTexture()
{
	return myTexture;
}

//Normalized you turd
void CRenderPackage::SetViewport(const CU::Vector4f& aRect)
{
	myViewport->TopLeftX = mySize.x * aRect.x;
	myViewport->TopLeftY = mySize.y * aRect.y;
	myViewport->Width	 = mySize.x * aRect.z;
	myViewport->Height	 = mySize.y * aRect.w;
}

void CRenderPackage::UpdateTexture(ID3D11Texture2D * aTexture)
{
	SAFE_RELEASE(myTexture);
	SAFE_RELEASE(myTarget);
	SAFE_RELEASE(myDepth);
	SAFE_RELEASE(myDepthResource);
	SAFE_RELEASE(myResource);

	HRESULT result;
	myTexture = aTexture;
	result = DEVICE->CreateRenderTargetView(myTexture, NULL, &myTarget);
	CHECK_RESULT(result, "Couldn't create render target view for the RenderPackage");
	FRAMEWORK->CreateDepthStencil(static_cast<unsigned int>(myViewport->Width), static_cast<unsigned int>(myViewport->Height), myDepth, myDepthResource);
}

CU::Vector2f CRenderPackage::GetSize()
{
	return CU::Vector2f(myViewport->Width, myViewport->Height);
}

void CRenderPackage::SaveToFile(const char* aPath)
{
	std::wstring path = CU::StringToWString(aPath);
	DirectX::SaveDDSTextureToFile(DEVICE_CONTEXT, myTexture, path.c_str());
}

void CRenderPackage::operator=(const CRenderPackage& aRight)
{
	SAFE_RELEASE(myTexture);
	myTexture = aRight.myTexture;
	SAFE_ADD_REF(myTexture);

	SAFE_RELEASE(myDepthResource);
	myDepthResource = aRight.myDepthResource;
	SAFE_ADD_REF(myDepthResource);

	SAFE_RELEASE(myResource);
	myResource = aRight.myResource;
	SAFE_ADD_REF(myResource);

	SAFE_RELEASE(myTarget);
	myTarget = aRight.myTarget;
	SAFE_ADD_REF(myTarget);

	SAFE_RELEASE(myDepth);
	myDepth = aRight.myDepth;
	SAFE_ADD_REF(myDepth);

	SAFE_DELETE(myViewport);
	if (aRight.myViewport != nullptr)
		myViewport = new D3D11_VIEWPORT(*aRight.myViewport);
}




void CRenderPackage::CreateTexture2D(const int aWidth, const int aHeight, DXGI_FORMAT aFormat)
{
	D3D11_TEXTURE2D_DESC textureDesc;
	ZeroMemory(&textureDesc, sizeof(textureDesc));

	textureDesc.Width = aWidth;
	textureDesc.Height = aHeight;
	textureDesc.MipLevels = 1;
	textureDesc.ArraySize = 1;
	textureDesc.Format = aFormat;
	textureDesc.SampleDesc.Count = 1;
	textureDesc.SampleDesc.Quality = 0;
	textureDesc.Usage = D3D11_USAGE_DEFAULT;
	textureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	textureDesc.CPUAccessFlags = 0;
	textureDesc.MiscFlags = 0;

	HRESULT result = DEVICE->CreateTexture2D(&textureDesc, NULL, &myTexture);
	CHECK_RESULT(result, "Failed to create Texture2D for the renderPackage.");
}

void CRenderPackage::CreateDepthTexture2D(const int aWidth, const int aHeight, DXGI_FORMAT aFormat)
{
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	// Set up the description of the depth buffer.
	depthBufferDesc.Width = aWidth;
	depthBufferDesc.Height = aHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = FRAMEWORK->GetDepthResourceFormat(aFormat);
	depthBufferDesc.SampleDesc.Count = 1;				//MSAA magic set too 4 for better stuffs
	depthBufferDesc.SampleDesc.Quality = 0;				//MSAA magic set to 1 to enable magic
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL | D3D11_BIND_SHADER_RESOURCE;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	HRESULT result = DEVICE->CreateTexture2D(&depthBufferDesc, NULL, &myDepthTexture);
	CHECK_RESULT(result, "Failed to create Texture2D for the renderPackage.");
}

#include "stdafx.h"
#include "Cubemap.h"
#include "Engine.h"
#include "DXFramework.h"
#include "RenderMessages.h"
#include "Renderer.h"
#include "TextureManager.h"

CCubemap::CCubemap()
{
	for (int i = 0; i < 6; ++i)
	{
		myRTV[i] = nullptr;
	}
	mySRV = nullptr;
	myTexture = nullptr;
	myViewport = nullptr;
	Init();
	myIsInEngine = true;
}


CCubemap::CCubemap(const char* aPath)
{
	for (int i = 0; i < 6; ++i)
	{
		myRTV[i] = nullptr;
	}
	mySRV = nullptr;
	myTexture = nullptr;
	myViewport = nullptr;

	myTextureFromFile = &TEXTUREMGR.LoadTexture(aPath);
	myIsInEngine = false;
}

CCubemap::~CCubemap()
{
	for (int i = 0; i < 6; ++i)
	{
		SAFE_RELEASE(myRTV[i]);
	}
	SAFE_RELEASE(mySRV);
	SAFE_RELEASE(myTexture);
	SAFE_DELETE(myViewport);

	if (myTextureFromFile != nullptr)
		TEXTUREMGR.DestroyTexture(myTextureFromFile);
}

void CCubemap::Init()
{
	myViewport = new D3D11_VIEWPORT();
	myViewport->Height = height;
	myViewport->Width = width;
	myViewport->MaxDepth = 1.0f;
	myViewport->MinDepth = 0.0f;
	myViewport->TopLeftX = 0.0f;
	myViewport->TopLeftY = 0.0f;

	D3D11_TEXTURE2D_DESC texDesc;
	ZeroMemory(&texDesc, sizeof(texDesc));
	texDesc.Width = (unsigned int)width;
	texDesc.Height = (unsigned int)height;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 6;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texDesc.CPUAccessFlags = 0;
	texDesc.SampleDesc.Count = 1;
	texDesc.SampleDesc.Quality = 0;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
	texDesc.CPUAccessFlags = 0;
	texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	HRESULT result = DEVICE->CreateTexture2D(&texDesc, NULL, &myTexture);
	CHECK_RESULT(result,"Tex2D Womba");



	D3D11_SHADER_RESOURCE_VIEW_DESC SRVDesc;
	ZeroMemory(&SRVDesc, sizeof(SRVDesc));
	SRVDesc.Format = texDesc.Format;
	SRVDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	SRVDesc.TextureCube.MipLevels = texDesc.MipLevels;
	SRVDesc.TextureCube.MostDetailedMip = 0;

	result = DEVICE->CreateShaderResourceView(myTexture, &SRVDesc, &mySRV);
	CHECK_RESULT(result, "SRV Womba");


	D3D11_RENDER_TARGET_VIEW_DESC RTVdesc;
	ZeroMemory(&RTVdesc, sizeof(RTVdesc));
	RTVdesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	RTVdesc.Texture2DArray.ArraySize = 1;
	RTVdesc.Texture2DArray.MipSlice = 0;
	RTVdesc.Format = texDesc.Format;

	for (int i = 0; i < 6; ++i)
	{
		RTVdesc.Texture2DArray.FirstArraySlice = i;
		result = DEVICE->CreateRenderTargetView(myTexture, &RTVdesc, &myRTV[i]);
		CHECK_RESULT(result, "RTV: %i Womba", i);
	}
}

void CCubemap::Clear()
{
	if (myIsInEngine)
	{
		SClear msg;
		for (int i = 0; i < 6; ++i)
		{
			msg.myRTV = myRTV[i];
			RENDERER.AddRenderMessage(new SClear(msg));
		}
	}
}

void CCubemap::SetShaderResource()
{
	SSetCubemapResource msg;
	msg.mySRV = myIsInEngine ? mySRV : myTextureFromFile->GetShaderResourceView();
	RENDERER.AddRenderMessage(new SSetCubemapResource(msg));
}

void CCubemap::ActivateForRender(const int aIndex)
{
	if (myIsInEngine)
	{
		SSetRTVMessage *msg = new SSetRTVMessage();
		msg->myRTV = myRTV[aIndex];
		msg->myViewport = myViewport;
		RENDERER.AddRenderMessage(msg);
	}
}

ID3D11ShaderResourceView* CCubemap::GetSRV()
{
	return myIsInEngine ? mySRV : myTextureFromFile->GetShaderResourceView();
}

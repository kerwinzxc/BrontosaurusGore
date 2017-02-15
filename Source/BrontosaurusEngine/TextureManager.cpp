#include "stdafx.h"
#include "TextureManager.h"
#include "DDSTextureLoader.h"
#include "Engine.h"
#include "dds.h"
#include <StringHelper.h>
#include "DXFramework.h"
#include <memory>
//#include <mutex>

CTextureManager::CTextureManager()
{
}


CTextureManager::~CTextureManager()
{
	assert(myTextures.size() == 0);
}

CTexture & CTextureManager::LoadTexture(const wchar_t* aWideTexturePath)
{
	//std::mutex loadMutex;
	//loadMutex.lock();
	if (myTextures.find(aWideTexturePath) == myTextures.end())
	{
		myTextures[aWideTexturePath] = CTexture();
		CreateTexture(aWideTexturePath, &myTextures[aWideTexturePath]);
	}

	myTextures[aWideTexturePath].AddRef();

#ifdef _DEBUG
	myTextures[aWideTexturePath].myPath = aWideTexturePath;
#endif


	return myTextures[aWideTexturePath];
}

CTexture& CTextureManager::LoadTexture(const char* aTexturePath)
{
	wchar_t wBuffer[1024];
	const wchar_t* path = CU::CharToWChar(wBuffer, aTexturePath);

	return LoadTexture(path);
}

void CTextureManager::DestroyTexture(const CTexture* aTexture)
{
	if (aTexture == nullptr)
	{
		return;
	}

	for (auto it = myTextures.begin(); it != myTextures.end(); ++it)
	{
		if (&it->second == aTexture)
		{
			int refCount = it->second.DecRef();
			if (refCount < 1)
			{
				myTextures.erase(it);
			}

			break;
		}
	}
}

void CTextureManager::CreateTexture(const wchar_t* aTexturePath, CTexture* aNewTexture)
{
	HRESULT result = S_OK;
	ID3D11ShaderResourceView* texture = nullptr;
	ID3D11Texture2D *pTextureInterface = nullptr;
	ID3D11Resource *res = nullptr;
	//exists?
	std::ifstream infile(aTexturePath);
	if (!infile.good())
	{
		aTexturePath = L"error.dds";
	}

	result = DirectX::CreateDDSTextureFromFile(DEVICE, aTexturePath, &res, &texture);
	if (FAILED(result))
	{
		RESOURCES_LOG("Could not create or load texture: %ls", aTexturePath);
		DL_PRINT_WARNING("Could not create or load texture: %ls", aTexturePath);
		aNewTexture->myTexture = nullptr;
		aNewTexture->myMipMapCount = 0;
		aNewTexture->mySize.x = 0;
		aNewTexture->mySize.y = 0;
		return;
	}
	
	//CHECK_RESULT(result, "Could not create or load texture: %s", aTexturePath);

	texture->GetResource(&res);
	result = res->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
	//CHECK_RESULT(result, "Could not get texture interface: %s", aTexturePath);
	if (FAILED(result))
	{
		RESOURCES_LOG("Could not get texture interface: %ls", aTexturePath);
	}

	D3D11_TEXTURE2D_DESC desc;
	pTextureInterface->GetDesc(&desc);

	aNewTexture->myTexture = texture;
	aNewTexture->myMipMapCount = desc.MipLevels;
	aNewTexture->mySize.x = desc.Width;
	aNewTexture->mySize.y = desc.Height;

	res->Release();
	pTextureInterface->Release();

}

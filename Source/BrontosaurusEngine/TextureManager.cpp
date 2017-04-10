#include "stdafx.h"
#include "TextureManager.h"
#include "DDSTextureLoader.h"
#include "Engine.h"
#include <StringHelper.h>
#include "DXFramework.h"
//#include <mutex>

CTextureManager::CTextureManager()
{
}


CTextureManager::~CTextureManager()
{
	//assert(myTextures.size() == 0);
}

CTexture & CTextureManager::LoadTexture(const wchar_t* aWideTexturePath)
{
	//std::mutex loadMutex;
	//loadMutex.lock();
	if (myTextures.find(aWideTexturePath) == myTextures.end())
	{
		myTextures[aWideTexturePath] = CTexture();
		CreateTexture(aWideTexturePath, myTextures[aWideTexturePath]);
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

void CTextureManager::DestroyTexture(CTexture* aTexture)
{
	if (aTexture == nullptr)
	{
		return;
	}

	if (aTexture->DecRef() <= 0)
	{
		for (auto it = myTextures.begin(); it != myTextures.end(); ++it)
		{
			if (&it->second == aTexture)
			{
				myTextures.erase(it);
				break;
			}
		}
	}
}

void CTextureManager::CreateTexture(const wchar_t* aTexturePath, CTexture& aNewTexture)
{
	HRESULT result = S_OK;

	//exists?
#ifndef _RETAIL_BUILD
	std::ifstream infile(aTexturePath);
	if (!infile.good())
	{
		DL_PRINT_WARNING("Failed to load texture %S", aTexturePath);
		aTexturePath = L"error.dds";
	}
#endif // !_RETAIL_BUILD

	ID3D11Resource *textureResource = nullptr;
	ID3D11ShaderResourceView* shaderResourceView = nullptr;
	result = DirectX::CreateDDSTextureFromFile(DEVICE, aTexturePath, &textureResource, &shaderResourceView);
	if (FAILED(result))
	{
		RESOURCES_LOG("Could not create or load texture: %ls", aTexturePath);
		DL_PRINT_WARNING("Could not create or load texture: %ls", aTexturePath);
		aNewTexture.myTexture = nullptr;
		aNewTexture.myMipMapCount = 0;
		aNewTexture.mySize.x = 0;
		aNewTexture.mySize.y = 0;
		return;
	}

	aNewTexture.myTexture = shaderResourceView;

	ID3D11Texture2D *pTextureInterface = nullptr;
	result = textureResource->QueryInterface<ID3D11Texture2D>(&pTextureInterface);
	if (FAILED(result))
	{
		RESOURCES_LOG("Could not get texture interface: %ls", aTexturePath);
		RESOURCES_LOG("Loaded DDS could be 3D texture: %ls", aTexturePath);

		
		return;
	
	}
	D3D11_TEXTURE2D_DESC desc;
	pTextureInterface->GetDesc(&desc);

	aNewTexture.myMipMapCount = desc.MipLevels;
	aNewTexture.mySize.x = desc.Width;
	aNewTexture.mySize.y = desc.Height;

	SAFE_RELEASE(textureResource);
	SAFE_RELEASE(pTextureInterface);
}

#pragma once
#include "Texture.h"
#include <unordered_map>


class CTextureManager
{
public:
	CTextureManager();
	~CTextureManager();

	CTexture& LoadTexture(const wchar_t* aWideTexturePath);
	CTexture& LoadTexture(const char* aTexturePath);
	void DestroyTexture(const CTexture* aTexture);
	
private:
	void CreateTexture(const wchar_t* aTexturePath, CTexture * aNewTexture);
private:
	std::unordered_map<std::wstring, CTexture> myTextures;
};

#pragma once
#include "Camera.h"


namespace
{
	float width, height = width = 1024;
}

struct ID3D11Texture2D;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;
struct ID3D11DepthStencilView;
struct D3D11_VIEWPORT;

class CTexture;

class CCubemap
{
public:
	CCubemap();
	CCubemap(const char* aPath);

	~CCubemap();
	void ActivateForRender(const int aIndex);
	ID3D11ShaderResourceView* GetSRV();
	void Clear();
	void SetShaderResource();

private:
	void Init();
	ID3D11Texture2D* myTexture;
	ID3D11ShaderResourceView* mySRV;
	ID3D11RenderTargetView* myRTV[6];
	D3D11_VIEWPORT* myViewport;
	//ID3D11DepthStencilView* myDepth;

	CTexture* myTextureFromFile;
	bool myIsInEngine;
};


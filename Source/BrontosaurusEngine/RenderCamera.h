#pragma once
#include <Camera.h>
#include "RenderPackage.h"

struct  SRenderMessage;

class CRenderCamera
{
public:
	CRenderCamera();
	~CRenderCamera();

	void InitPerspective(const float aFov, const float aWidth, const float aHeight, const float aFar, const float aNear, ID3D11Texture2D * aTexture = nullptr, DXGI_FORMAT aFormat = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
	void InitOrthographic(const float aWidth, const float aHeight, const float aFar, const float aNear, const int aTextureWidth, const int aTextureHeight, ID3D11Texture2D * aTexture = nullptr, DXGI_FORMAT aFormat = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
	
	inline void SetCamera(const CU::Camera& aCamera);
	inline CU::Camera& GetCamera();

	inline CRenderPackage& GetRenderPackage();


	void AddRenderMessage(SRenderMessage* aRenderMessage);
	void Render();

private:
	CU::Camera myCamera;
	CRenderPackage myRenderPackage;
	CU::GrowingArray<SRenderMessage*, unsigned int, false> myRenderQueue;
	bool myIsShadowCamera;
};

inline CRenderPackage & CRenderCamera::GetRenderPackage() 
{
	return myRenderPackage;
}

inline void CRenderCamera::SetCamera(const CU::Camera & aCamera)
{
	myCamera = aCamera;
}

inline CU::Camera & CRenderCamera::GetCamera()
{
	return myCamera;
}

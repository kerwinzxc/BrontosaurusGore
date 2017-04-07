#pragma once
#include <Camera.h>
#include "RenderPackage.h"

struct SRenderMessage;
class CGeometryBuffer;
struct ID3D11PixelShader;

class CRenderCamera
{
public:
	friend class CScene;
	friend class CRenderer;
	CRenderCamera(const bool deferred = true);
	~CRenderCamera();

	void InitPerspective(const float aFov, const float aWidth, const float aHeight, const float aFar, const float aNear, ID3D11Texture2D * aTexture = nullptr, DXGI_FORMAT aFormat = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
	void InitOrthographic(const float aWidth, const float aHeight, const float aFar, const float aNear, const unsigned int aTextureWidth, const unsigned int aTextureHeight, ID3D11Texture2D * aTexture = nullptr, DXGI_FORMAT aFormat = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM);
	void ShadowInit();

	inline void SetCamera(const CU::Camera& aCamera);
	inline CU::Camera& GetCamera();

	inline CRenderPackage& GetRenderPackage();
	inline bool GetIsShadowCamera() const;

	void SetViewport(const CU::Vector4f& aRect);
	void AddRenderMessage(SRenderMessage* aRenderMessage);
	void Render();

	ID3D11PixelShader* GetShadowShader();
private:
	void InitRenderPackages(const CU::Vector2ui& aTextureSize, ID3D11Texture2D* aTexture, DXGI_FORMAT aFormat);

private:
	CGeometryBuffer* myGbuffer;
	CU::Camera myCamera;
	CRenderPackage myRenderPackage;
	CU::GrowingArray<SRenderMessage*, unsigned int, false> myRenderQueue;
	ID3D11PixelShader* myShadowPS;
	ID3D11PixelShader* myShadowPSInstanced;

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

inline bool CRenderCamera::GetIsShadowCamera() const
{
	return myIsShadowCamera;
}

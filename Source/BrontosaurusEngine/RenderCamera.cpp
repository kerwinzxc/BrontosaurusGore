#include "stdafx.h"
#include "RenderCamera.h"
#include "Renderer.h"
#include "Engine.h"
#include "ShaderManager.h"
#include "RenderMessages.h"

#include "GeometryBuffer.h"
#include "EModelBluePrint.h"

CRenderCamera::CRenderCamera(const bool aDeferred /*= true*/)
{
	myRenderQueue.Init(32);
	myIsShadowCamera = false;
	myShadowPS = nullptr;
	myShadowPSInstanced = nullptr;
	myGbuffer = nullptr;

	if (aDeferred == true)
	{
		myGbuffer = new CGeometryBuffer();
	}
}

CRenderCamera::~CRenderCamera()
{
	myRenderQueue.DeleteAll();
	//SAFE_DELETE(myGbuffer);
	//SAFE_RELEASE(myShadowPS);
	//SAFE_RELEASE(myShadowPSInstanced);
}

void CRenderCamera::InitPerspective(const float aFov, const float aWidth, const float aHeight, const float aFar, const float aNear, ID3D11Texture2D* aTexture , DXGI_FORMAT aFormat /*= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM*/)
{
	myCamera.Init(aFov, aWidth, aHeight, aFar, aNear);
	CU::Vector2ui size(aWidth, aHeight);
	InitRenderPackages(size, aTexture, aFormat);
}

//ORTOGRAPHIC
void CRenderCamera::InitOrthographic(const float aWidth, const float aHeight, const float aFar, const float aNear, const unsigned int aTextureWidth, const unsigned int aTextureHeight, ID3D11Texture2D* aTexture, DXGI_FORMAT aFormat /*= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM*/)
{
	myCamera.Init(aWidth, aHeight, aNear, aFar);
	InitRenderPackages({ aTextureWidth, aTextureHeight }, aTexture, aFormat);
}

void CRenderCamera::ShadowInit()
{
	myIsShadowCamera = true;
	myShadowPS = SHADERMGR->LoadPixelShader(L"Shaders/shadow.fx", 3);
	myShadowPSInstanced = SHADERMGR->LoadPixelShader(L"Shaders/shadow.fx", 3 | EModelBluePrint_Instance);
}

void CRenderCamera::SetViewport(const CU::Vector4f& aRect)
{
	myRenderPackage.SetViewport(aRect);
}

void CRenderCamera::AddRenderMessage(SRenderMessage * aRenderMessage)
{
	myRenderQueue.Add(aRenderMessage);
}

void CRenderCamera::Render()
{
	SRenderCameraQueueMessage * camqueueMsg = new SRenderCameraQueueMessage();
	camqueueMsg->myRenderCamera = *this;
	RENDERER.AddRenderMessage(camqueueMsg);
	myRenderQueue.RemoveAll();
}

ID3D11PixelShader* CRenderCamera::GetShadowShader()
{
	return myShadowPS;
}

void CRenderCamera::InitRenderPackages(const CU::Vector2ui& aTextureSize, ID3D11Texture2D* aTexture, DXGI_FORMAT aFormat)
{
	myRenderPackage.Init(aTextureSize, aTexture, aFormat);
	if (myGbuffer != nullptr)
	{
		CEngine* engine = CEngine::GetInstance();
		if (!engine)
		{
			DL_ASSERT("Failed to get engine");
		}
		CDXFramework* framework = engine->GetFramework();
		if (!framework)
		{
			DL_ASSERT("Failed to get framework");
		}
		myGbuffer->Init(aTextureSize, framework, aTexture, aFormat);
	}
}


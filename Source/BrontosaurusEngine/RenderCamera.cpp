#include "stdafx.h"
#include "RenderCamera.h"
#include "Renderer.h"
#include "Engine.h"
#include "RenderMessages.h"


CRenderCamera::CRenderCamera()
{
	myRenderQueue.Init(32);
	myIsShadowCamera = false;
}


CRenderCamera::~CRenderCamera()
{
	myRenderQueue.DeleteAll();
}

void CRenderCamera::InitPerspective(const float aFov, const float aWidth, const float aHeight, const float aFar, const float aNear, ID3D11Texture2D* aTexture , DXGI_FORMAT aFormat /*= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM*/)
{
	myCamera.Init(aFov, aWidth, aHeight, aNear, aFar);
	CU::Vector2ui size(aWidth, aHeight);
	myRenderPackage.Init(size, aTexture, aFormat);
}

//ORTOGRAPHIC
void CRenderCamera::InitOrthographic(const float aWidth, const float aHeight, const float aFar, const float aNear, const int aTextureWidth, const int aTextureHeight, ID3D11Texture2D* aTexture, DXGI_FORMAT aFormat /*= DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM*/)
{
	myCamera.Init(aWidth, aHeight, aNear, aFar);
	CU::Vector2ui size(aTextureWidth, aTextureHeight);
	myRenderPackage.Init(size, aTexture, aFormat);
}

void CRenderCamera::AddRenderMessage(SRenderMessage * aRenderMessage)
{
	myRenderQueue.Add(aRenderMessage);
}

void CRenderCamera::Render()
{
	SRenderCameraQueueMessage * camqueueMsg = new SRenderCameraQueueMessage();
	camqueueMsg->myCamera = myCamera;
	camqueueMsg->CameraRenderPackage = myRenderPackage;
	camqueueMsg->CameraRenderQueue = myRenderQueue;
	RENDERER.AddRenderMessage(camqueueMsg);
	myRenderQueue.RemoveAll(); // these are deleted on the render thread
}


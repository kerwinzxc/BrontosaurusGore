#include "stdafx.h"
#include "Renderer.h"
#include "DXFramework.h"
#include "Engine.h"
#include "PointLightInstance.h"
#include "Lights.h"
#include "Model.h"
#include "Sprite.h"
#include "Skybox.h"
#include "StreakEmitterInstance.h"
#include "ParticleEmitter.h"
#include "ParticleEmitterManager.h"
#include "FireEmitterManager.h"
#include "FireEmitter.h"
#include "ModelManager.h"
#include "ConstBufferTemplate.h"
#include "WindowsWindow.h"
#include <Camera.h>

#include "ModelInstance.h"
#include "../PostMaster/PostMaster.h"
#include "../PostMaster/Message.h"
#include "../PostMaster/DrawCallsCount.h"

#include "../GUI/GUIPixelConstantBuffer.h"
#include "LineDrawer.h"

#define HDR_FORMAT DXGI_FORMAT_R32G32B32A32_FLOAT

CRenderer::CRenderer()
{
	myIsRunning = true;

	mySettings.HDR = true;
	mySettings.Bloom = false;
	mySettings.Motionblur = false;
	mySettings.CromaticAberration = false;

	myOncePerFrameBufferTimer = myTimers.CreateTimer();
	myFireTimer = myTimers.CreateTimer();

	CreateBuffer();

	InitPackages();
	CreateRasterizerStates();
	CreateBlendStates();
	CreateDepthStencilStates();
	CreateSamplerStates();


	SChangeStatesMessage changeStateMessage = {};
	changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
	changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
	changeStateMessage.myBlendState = eBlendState::eNoBlend;
	changeStateMessage.mySamplerState = eSamplerState::eClamp0Wrap1;

	SetStates(&changeStateMessage);
}

CRenderer::~CRenderer()
{
	for (int i = 0; i < myRasterizerStates.Size(); ++i)
	{
		myRasterizerStates[i]->Release();
		myRasterizerStates[i] = nullptr;
	}

	for (int i = 0; i < myDepthStencilStates.Size(); ++i)
	{
		myDepthStencilStates[i]->Release();
		myDepthStencilStates[i] = nullptr;
	}

	for (int i = 0; i < myBlendStates.Size(); ++i)
	{
		myBlendStates[i]->Release();
		myBlendStates[i] = nullptr;
	}

	mySynchronizer.ClearAll();
	myLightSynchronizer.ClearAll();
}

void CRenderer::Shutdown()
{
	myIsRunning = false;
}

void CRenderer::AddRenderMessage(SRenderMessage* aRenderMessage)
{
	mySynchronizer << aRenderMessage;
}

void CRenderer::DoDefered()
{
	gBuffer.Clear();
	gBuffer.SetBuffer();
	DoRenderQueue();
}

void CRenderer::GBuffer::Init()
{
	
		const CU::Vector2ui size(1920, 1080);
		albedoPackage.Init(size, nullptr);
		normalPackage.Init(size, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
		depthPackage.Init(size, nullptr, DXGI_FORMAT_R32_FLOAT);
	
}

void CRenderer::GBuffer::SetBuffer()
{
	
		ID3D11DeviceContext* context = DEVICE_CONTEXT;
		
		ID3D11RenderTargetView* targets[3]{ albedoPackage.GetTarget(), normalPackage.GetTarget(), depthPackage.GetTarget() };
		context->OMSetRenderTargets(3, targets, albedoPackage.GetDepth());
		context->RSSetViewports(1, albedoPackage.GetViewport());
	
}

void CRenderer::GBuffer::Clear()
{
	albedoPackage.Clear();
	normalPackage.Clear();
	depthPackage.Clear();
}

void CRenderer::GBuffer::SetAsResource()
{
	albedoPackage.SetResource(1);
	normalPackage.SetResource(2);
	albedoPackage.SetDepthAsResource(3);
}

CRenderPackage& CRenderer::GBuffer::GetAlbedo()
{
	return albedoPackage;
}

CRenderPackage& CRenderer::GBuffer::GetNormal()
{
	return normalPackage;
}

ID3D11ShaderResourceView* CRenderer::GBuffer::GetDepth()
{
	return albedoPackage.GetDepthResource();
}

void CRenderer::DoLight()
{
	if(!myLightSynchronizer == 0)
	{
		return;
	}
	myFullScreenHelper.Activate(CFullScreenHelper::eEffectType::ePBL);
	gBuffer.SetAsResource();
	ID3D11ShaderResourceView* pckg = nullptr;

	const int numberOfLights = myLightSynchronizer[0]->Size();
	for(int i = 0; i < numberOfLights; ++i)
	{
		UpdatePointlightBuffer((*myLightSynchronizer[0])[i]);
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::ePBL, pckg);
	}
}

void CRenderer::Render()
{

	myLightSynchronizer.SwapRead();
	myTimers.UpdateTimers();
	UpdateBuffer();

	myBackBufferPackage.Clear();
	myIntermediatePackage.Clear();
	myGUIData.myInputPackage.Clear();

	DoDefered();

	renderTo = (mySettings.HDR == true) ? &myHDRData.myInputPackage : &myIntermediatePackage;
	renderTo->Clear();

	if (mySettings.Motionblur == true)
	{
		myMotionBlurData.velocityPackage.Clear();
		myMotionBlurData.inputPackage.Clear();
		renderTo->Activate(myMotionBlurData.velocityPackage);
	}
	else
	{
		renderTo->Activate();
	}

	DoLight();
	

	SChangeStatesMessage changeStateMessage = {};
	changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
	changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
	changeStateMessage.myBlendState = eBlendState::eNoBlend;
	changeStateMessage.mySamplerState = eSamplerState::eClamp;

	SetStates(&changeStateMessage);

	Downsample(*renderTo);
	HDR();
	Bloom();
	MotionBlur();
	LensDistortion(myIntermediatePackage);
	RenderGUI();


	myBackBufferPackage.Activate();
	myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myIntermediatePackage);

	/*myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, CU::Vector4f(0,0,0.33,0.33), &gBuffer.GetAlbedo());

	myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, CU::Vector4f(0.33, 0., 0.66, 0.33), &gBuffer.GetNormal());

	myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, CU::Vector4f(0.66, 0., 1.0, 0.33), gBuffer.GetDepth());*/
}

void CRenderer::SwapWrite()
{
	mySynchronizer.SwapWrite();
	myLightSynchronizer.SwapWrite();
}

void CRenderer::Bloom()
{
	if (mySettings.Bloom == true)
	{
		myBloomData.myInputPackage.Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myIntermediatePackage);


		myBloomData.myLumPackage.Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eBloominance, &myDownsampleData.downsamplePackages[0]);

		myBloomData.myBloomPackages[0].Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eGaussianBlurHorizontal, &myBloomData.myLumPackage);
		myBloomData.myBloomPackages[1].Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eGaussianBlurVertical, &myBloomData.myBloomPackages[0]);
		myBloomData.myBloomPackages[2].Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eGaussianBlurHorizontal, &myBloomData.myBloomPackages[1]);
		myBloomData.myBloomPackages[3].Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eGaussianBlurVertical, &myBloomData.myBloomPackages[2]);
		myIntermediatePackage.Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eAdd, &myBloomData.myBloomPackages[1], &myBloomData.myInputPackage);
	}
}

void CRenderer::HDR()
{
	if (mySettings.HDR == true)
	{
		myHDRData.myAverageLumPackage.Clear();
		myHDRData.myAverageLumPackage.Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eLuminance, &myDownsampleData.downsamplePackages.GetLast());

		myHDRData.myTonemapPackage.Clear();
		myHDRData.myTonemapPackage.Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eToneMap, &myHDRData.myInputPackage, &myHDRData.myAverageLumPackage);

		myIntermediatePackage.Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myHDRData.myTonemapPackage);
	}
}

void CRenderer::MotionBlur()
{
	if (mySettings.Motionblur == true)
	{
		myMotionBlurData.inputPackage.Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myIntermediatePackage);
		myIntermediatePackage.Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eMotionBlur, &myMotionBlurData.inputPackage, &myMotionBlurData.velocityPackage);
	}
}

void CRenderer::RenderGUI()
{
	SChangeStatesMessage changeStateMessage = {};
	changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
	changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
	changeStateMessage.myBlendState = eBlendState::eAlphaBlend;
	changeStateMessage.mySamplerState = eSamplerState::eClamp;

	SetStates(&changeStateMessage);

	if (myGUIData.myShouldDownScale == true)
	{
		for (int i = 0; i < myGUIData.myDownScalePackages.Size(); ++i)
		{
			myGUIData.myDownScalePackages[i].Clear();
		}

		myGUIData.myDownScalePackages[0].Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGUIData.myInputPackage);

		myGUIData.myDownScalePackages[1].Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGUIData.myDownScalePackages[0]);

		myGUIData.myDownScalePackages[2].Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGUIData.myDownScalePackages[1]);

		myIntermediatePackage.Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGUIData.myDownScalePackages[2]);
	}
	else
	{
		myIntermediatePackage.Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGUIData.myInputPackage);
	}

	changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
	changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
	changeStateMessage.myBlendState = eBlendState::eNoBlend;
	SetStates(&changeStateMessage);
}

void CRenderer::LensDistortion(CRenderPackage & aRenderMessage)
{
	if (mySettings.CromaticAberration)
	{
		myDistortionData.inputPackage.Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &aRenderMessage);
		myDistortionData.distortionPackage.Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCubicLensDistortion, &myDistortionData.inputPackage);
		aRenderMessage.Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myDistortionData.distortionPackage);

	}
}

void CRenderer::ClearRenderQueue()
{
	mySynchronizer.ClearAll();
	myLightSynchronizer.ClearAll();
}

void CRenderer::Downsample(CRenderPackage & aRenderPackage)
{
	myDownsampleData.squarePackage.Activate();
	myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &aRenderPackage);

	myDownsampleData.downsamplePackages[0].Activate();
	myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myDownsampleData.squarePackage);

	unsigned int numberOfDownsamples = myDownsampleData.downsamplePackages.Size();
	for (unsigned int i = 1; i < numberOfDownsamples; ++i)
	{
		myDownsampleData.downsamplePackages[i].Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myDownsampleData.downsamplePackages[i - 1]);
	}
}

void CRenderer::InitPackages()
{
	//CU::Vector2ui windowSize = ENGINE->GetWindowSize();
	CU::Vector2ui windowSize = ENGINE->GetWindow()->GetWindowSize();

	if (mySettings.HDR == true)
	{
		myIntermediatePackage.Init(windowSize, nullptr, HDR_FORMAT);

	}
	else
	{
		myIntermediatePackage.Init(windowSize);
	}


	{
		myDownsampleData.downsamplePackages.Init(16);

		unsigned int smallest = min(windowSize.x, windowSize.y);
		unsigned int downsample1 = static_cast<unsigned int>(pow(2, floor(log(smallest) / log(2))));

		myDownsampleData.squarePackage.Init({ smallest, smallest });

		unsigned int currentSize = downsample1;

		while (currentSize >= 2)
		{
			myDownsampleData.downsamplePackages.Add(CRenderPackage());
			myDownsampleData.downsamplePackages.GetLast().Init({ currentSize, currentSize });
			currentSize /= 2;
		}
	}


	//	HDR
	//
	{
		myHDRData.myInputPackage.Init(windowSize, nullptr, DXGI_FORMAT_R32G32B32A32_FLOAT);
		myHDRData.myTonemapPackage.Init(windowSize);
		myHDRData.myAverageLumPackage.Init({ (unsigned)2 , (unsigned)2 });
		myHDRData.myAverageColPackage.Init({ (unsigned)2 , (unsigned)2 });
	}

	//	BLOOM
	//
	{
		myBloomData.myInputPackage.Init(windowSize, nullptr, DXGI_FORMAT_R16G16B16A16_FLOAT);
		unsigned int bloomSize = static_cast<unsigned int>(myDownsampleData.downsamplePackages[0].GetSize().x);

		if (mySettings.HDR == true)
		{
			myBloomData.myLumPackage.Init({ bloomSize, bloomSize }, nullptr, HDR_FORMAT);
		}
		else
		{
			myBloomData.myLumPackage.Init({ bloomSize, bloomSize });
		}

		myBloomData.myBloomPackages[0].Init({ bloomSize, bloomSize });
		myBloomData.myBloomPackages[1].Init({ bloomSize / 2, bloomSize / 2 });
		myBloomData.myBloomPackages[2].Init({ bloomSize / 4, bloomSize / 4 });
		myBloomData.myBloomPackages[3].Init({ bloomSize / 8, bloomSize / 8 });
	}



	//	MOTBLUR
	//
	{
		myMotionBlurData.inputPackage.Init(windowSize);
		myMotionBlurData.velocityPackage.Init(windowSize, nullptr, DXGI_FORMAT::DXGI_FORMAT_R16G16B16A16_FLOAT);
	}


	//
	//	LensDistortion
	{
		myDistortionData.inputPackage.Init(windowSize);
		myDistortionData.distortionPackage.Init(windowSize);
	}


	//  GUI
	//
	{
		myGUIData.myInputPackage.Init(windowSize);

		unsigned int minSize = min(windowSize.x, windowSize.y);
		myGUIData.myDownScalePackages[0].Init(CU::Vector2ui(minSize, minSize));

		unsigned int minPow2Size = 2u;
		while (minPow2Size * 2u < minSize)
		{
			minPow2Size *= 2u;
		}

		myGUIData.myDownScalePackages[1].Init(CU::Vector2ui(minPow2Size, minPow2Size));
		myGUIData.myDownScalePackages[2].Init(CU::Vector2ui(minPow2Size / 2u, minPow2Size / 2u));
		myGUIData.myShouldDownScale = false;
	}

	//	Backbuffer
	//
	{

		HRESULT result;
		ID3D11Texture2D* backBuffer = nullptr;
		result = FRAMEWORK->GetSwapChain().GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBuffer);
		CHECK_RESULT(result, "Failed to fetch backbuffer in renderer");
		myBackBufferPackage.Init(ENGINE->GetWindowSize(), backBuffer);
	}

	gBuffer.Init();
}

void CRenderer::CreateBuffer()
{
	SOncePerFrameBuffer buffer;
	buffer.myCameraMatrices.myCameraSpace = myCamera.GetTransformation();
	buffer.myCameraMatrices.myCameraSpaceInverse = myCamera.GetInverse();
	buffer.myCameraMatrices.myProjectionSpace = myCamera.GetProjection();
	buffer.myCameraMatrices.myProjectionInverse = myCamera.GetProjection().GetInverted();
	buffer.myShadowCameraMatrices.myCameraSpaceInverse = CU::Matrix44f::Zero;
	buffer.myShadowCameraMatrices.myProjectionSpace = CU::Matrix44f::Zero;

	buffer.deltaTime = 0.0f;
	
	buffer.time = 0.0f;
	buffer.deltaTime = 0.0f;

	buffer.fogStart = 0.0f;
	buffer.fogEnd = 0.0f;

	myOncePerFrameBuffer = BSR::CreateCBuffer<SOncePerFrameBuffer>(&buffer);

	SPointLightBuffer pointLightBuffer;
	pointLightBuffer.pointLight.color = CU::Vector3f(1, 1, 1);
	pointLightBuffer.pointLight.intensity = 1;
	pointLightBuffer.pointLight.position = CU::Vector3f(0, 0, 0);
	pointLightBuffer.pointLight.range = 1;

	myPointlightBuffer = BSR::CreateCBuffer<SPointLightBuffer>(&pointLightBuffer);
}

void CRenderer::UpdateBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedSubResource;
	ZeroMemory(&mappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	SOncePerFrameBuffer updatedBuffer;

	const CU::Matrix44f cameraInverse = myCamera.GetInverse();
	updatedBuffer.myCameraMatrices.myCameraSpaceInverse = cameraInverse;
	updatedBuffer.myCameraMatrices.myProjectionSpace = myCamera.GetProjection();
	const CU::Matrix44f camera = myCamera.GetTransformation();
	updatedBuffer.myCameraMatrices.myCameraSpace = camera;
	updatedBuffer.myCameraMatrices.myProjectionInverse = myCamera.GetProjectionInverse();

	updatedBuffer.myShadowCameraMatrices.myCameraSpaceInverse = myCamera.GetInverse();
	
	updatedBuffer.myShadowCameraMatrices.myProjectionSpace = myCamera.GetProjection();

	updatedBuffer.deltaTime = 0;//myTimers.GetTimer(myOncePerFrameBufferTimer).GetDeltaTime().GetSeconds();
	updatedBuffer.time = myTimers.GetTimer(myOncePerFrameBufferTimer).GetLifeTime().GetSeconds();
	if(!myLightSynchronizer > 0)
	{
		updatedBuffer.deltaTime = myLightSynchronizer[0]->Size();
	}

	updatedBuffer.fogStart = 0.0f;
	updatedBuffer.fogEnd = 0.0f;

	
	
	DEVICE_CONTEXT->Map(myOncePerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
	memcpy(mappedSubResource.pData, &updatedBuffer, sizeof(SOncePerFrameBuffer));
	DEVICE_CONTEXT->Unmap(myOncePerFrameBuffer, 0);
	DEVICE_CONTEXT->VSSetConstantBuffers(0, 1, &myOncePerFrameBuffer);
	DEVICE_CONTEXT->GSSetConstantBuffers(0, 1, &myOncePerFrameBuffer);
	DEVICE_CONTEXT->PSSetConstantBuffers(0, 1, &myOncePerFrameBuffer);

}

void CRenderer::UpdatePointlightBuffer(const CPointLightInstance& aPointLightInstance)
{
	D3D11_MAPPED_SUBRESOURCE subresource;
	ZeroMemory(&subresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	SPointLightBuffer lightBuffer;
	lightBuffer.cameraPos = myCamera.GetPosition();
	lightBuffer.directionalLight;
	lightBuffer.pointLight.position = aPointLightInstance.GetPosition();
	lightBuffer.pointLight.color = aPointLightInstance.GetColor();
	lightBuffer.pointLight.intensity = aPointLightInstance.GetInstensity();
	lightBuffer.pointLight.range = aPointLightInstance.GetRange() * 100;
	lightBuffer.cubemapMipcount = 11;

	DEVICE_CONTEXT->Map(myPointlightBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &subresource);
	memcpy(subresource.pData, &lightBuffer, sizeof(SPointLightBuffer));
	DEVICE_CONTEXT->Unmap(myPointlightBuffer, 0);


	DEVICE_CONTEXT->PSSetConstantBuffers(2, 1, &myPointlightBuffer);

}

void CRenderer::UpdateBuffer(SSetShadowBuffer* msg) 
{
	D3D11_MAPPED_SUBRESOURCE mappedSubResource;
	ZeroMemory(&mappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	SOncePerFrameBuffer updatedBuffer;

	updatedBuffer.myCameraMatrices.myCameraSpaceInverse = myCamera.GetInverse();
	updatedBuffer.myCameraMatrices.myProjectionSpace = myCamera.GetProjection();
	updatedBuffer.myCameraMatrices.myCameraSpace = myCamera.GetTransformation();
	updatedBuffer.myCameraMatrices.myProjectionInverse = myCamera.GetProjectionInverse();
	
	updatedBuffer.myShadowCameraMatrices.myCameraSpaceInverse = msg->myCameraTransformation;
	updatedBuffer.myShadowCameraMatrices.myProjectionSpace = msg->myCameraProjection;

	updatedBuffer.deltaTime = 0;//myTimers.GetTimer(myOncePerFrameBufferTimer).GetDeltaTime().GetSeconds();
	updatedBuffer.time = myTimers.GetTimer(myOncePerFrameBufferTimer).GetLifeTime().GetSeconds();
	if (!myLightSynchronizer > 0)
	{
		updatedBuffer.deltaTime = myLightSynchronizer[0]->Size();
	}

	updatedBuffer.fogStart = 0.0f;
	updatedBuffer.fogEnd = 0.0f;

	DEVICE_CONTEXT->Map(myOncePerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
	memcpy(mappedSubResource.pData, &updatedBuffer, sizeof(SOncePerFrameBuffer));
	DEVICE_CONTEXT->Unmap(myOncePerFrameBuffer, 0);

	DEVICE_CONTEXT->VSSetConstantBuffers(0, 1, &myOncePerFrameBuffer);
	DEVICE_CONTEXT->GSSetConstantBuffers(0, 1, &myOncePerFrameBuffer);
	DEVICE_CONTEXT->PSSetConstantBuffers(0, 1, &myOncePerFrameBuffer);

	DEVICE_CONTEXT->PSSetShaderResources(7, 1, &msg->myShadowBuffer.GetDepthResource());
}

void CRenderer::CreateRasterizerStates()
{
	HRESULT result;
	
	ID3D11RasterizerState* defaultState = nullptr;
	D3D11_RASTERIZER_DESC rasterFillDesc;
	ZeroMemory(&rasterFillDesc, sizeof(rasterFillDesc));
	rasterFillDesc.AntialiasedLineEnable = true;	//Här är nån typ av MSAA cooldude mannen
	rasterFillDesc.MultisampleEnable = true;		//
	rasterFillDesc.FillMode = D3D11_FILL_SOLID;
	rasterFillDesc.CullMode = D3D11_CULL_BACK;
	rasterFillDesc.DepthClipEnable = true;
	rasterFillDesc.FrontCounterClockwise = false;
	result = DEVICE->CreateRasterizerState(&rasterFillDesc, &defaultState);
	CHECK_RESULT(result, "Failed to create Rasterizer Default State.");
	myRasterizerStates[static_cast<int>(eRasterizerState::eDefault)] = defaultState;


	ID3D11RasterizerState* wireframeState = nullptr;
	D3D11_RASTERIZER_DESC rasterWireframeDesc;
	ZeroMemory(&rasterWireframeDesc, sizeof(rasterWireframeDesc));
	rasterWireframeDesc.FillMode = D3D11_FILL_WIREFRAME;
	rasterWireframeDesc.CullMode = D3D11_CULL_NONE;
	rasterWireframeDesc.DepthClipEnable = true;
	result = DEVICE->CreateRasterizerState(&rasterWireframeDesc, &wireframeState);
	CHECK_RESULT(result, "Failed to create Rasterizer Wireframe State.");
	myRasterizerStates[static_cast<int>(eRasterizerState::eWireFrame)] = wireframeState;



	ID3D11RasterizerState* noCullState = nullptr;
	D3D11_RASTERIZER_DESC rasterNoCullDesc;
	ZeroMemory(&rasterNoCullDesc, sizeof(rasterNoCullDesc));

	rasterNoCullDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterNoCullDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_NONE;
	rasterNoCullDesc.FrontCounterClockwise = FALSE;
	rasterNoCullDesc.DepthBias = 0;
	rasterNoCullDesc.SlopeScaledDepthBias = 0.f;
	rasterNoCullDesc.DepthBiasClamp = 0.f;
	rasterNoCullDesc.DepthClipEnable = TRUE;
	rasterNoCullDesc.ScissorEnable = FALSE;
	rasterNoCullDesc.MultisampleEnable = FALSE;
	rasterNoCullDesc.AntialiasedLineEnable = FALSE;


	result = DEVICE->CreateRasterizerState(&rasterNoCullDesc, &noCullState);
	CHECK_RESULT(result, "Failed to create Rasterizer Default State.");
	myRasterizerStates[static_cast<int>(eRasterizerState::eNoCulling)] = noCullState;



	ID3D11RasterizerState* noCullClockwiseState = nullptr;
	D3D11_RASTERIZER_DESC rasterNoCullClockDesc;
	ZeroMemory(&rasterNoCullClockDesc, sizeof(rasterNoCullClockDesc));
	rasterNoCullClockDesc.FillMode = D3D11_FILL_SOLID;
	rasterNoCullClockDesc.CullMode = D3D11_CULL_NONE;
	rasterNoCullClockDesc.DepthClipEnable = false;
	rasterNoCullClockDesc.FrontCounterClockwise = false;
	result = DEVICE->CreateRasterizerState(&rasterNoCullClockDesc, &noCullClockwiseState);
	CHECK_RESULT(result, "Failed to create Rasterizer Default State.");
	myRasterizerStates[static_cast<int>(eRasterizerState::eNoCullingClockwise)] = noCullClockwiseState;
	


	DEVICE_CONTEXT->RSSetState(myRasterizerStates[static_cast<int>(eRasterizerState::eDefault)]);
}

void CRenderer::CreateBlendStates()
{
	HRESULT result;

	ID3D11BlendState* blendState = NULL;
	{


		D3D11_BLEND_DESC blendDesc_AlphaBlend = {};

		blendDesc_AlphaBlend.AlphaToCoverageEnable = FALSE;
		blendDesc_AlphaBlend.IndependentBlendEnable = FALSE;

		blendDesc_AlphaBlend.RenderTarget[0].BlendEnable = TRUE;
		blendDesc_AlphaBlend.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		blendDesc_AlphaBlend.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc_AlphaBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		blendDesc_AlphaBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		blendDesc_AlphaBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc_AlphaBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		blendDesc_AlphaBlend.RenderTarget[0].RenderTargetWriteMask = 0x0f;


		blendDesc_AlphaBlend.RenderTarget[0].BlendEnable = TRUE;
		blendDesc_AlphaBlend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc_AlphaBlend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc_AlphaBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc_AlphaBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blendDesc_AlphaBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc_AlphaBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc_AlphaBlend.RenderTarget[0].RenderTargetWriteMask = D3D10_COLOR_WRITE_ENABLE_ALL;


		//THIS ONE MAKES EVERYTHING WEIRD AF:
		//blendDesc_AlphaBlend.RenderTarget[0].BlendEnable = TRUE;
		//blendDesc_AlphaBlend.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
		//blendDesc_AlphaBlend.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
		//blendDesc_AlphaBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		//blendDesc_AlphaBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		//blendDesc_AlphaBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
		//blendDesc_AlphaBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		//blendDesc_AlphaBlend.RenderTarget[0].RenderTargetWriteMask = 0x0f;


		result = DEVICE->CreateBlendState(&blendDesc_AlphaBlend, &blendState);
		CHECK_RESULT(result, "Failed to create Alpha Blend State.");
		myBlendStates[static_cast<int>(eBlendState::eAlphaBlend)] = blendState;
	}

	blendState = NULL;
	{
		D3D11_BLEND_DESC blendDesc_NoBlend = {};

		blendDesc_NoBlend.AlphaToCoverageEnable = FALSE;
		blendDesc_NoBlend.IndependentBlendEnable = FALSE;

		blendDesc_NoBlend.RenderTarget[0].BlendEnable = FALSE;
		blendDesc_NoBlend.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_ONE;
		blendDesc_NoBlend.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_ZERO;
		blendDesc_NoBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		blendDesc_NoBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		blendDesc_NoBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		blendDesc_NoBlend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		blendDesc_NoBlend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		result = DEVICE->CreateBlendState(&blendDesc_NoBlend, &blendState);
		CHECK_RESULT(result, "Failed to create No-Blend State.");
		myBlendStates[static_cast<int>(eBlendState::eNoBlend)] = blendState;

	}
}

void CRenderer::CreateDepthStencilStates()
{
	HRESULT result;
	ID3D11DepthStencilState* depthStencilState = nullptr;

	{
		D3D11_DEPTH_STENCIL_DESC depthStencilEnabledDesc;
		ZeroMemory(&depthStencilEnabledDesc, sizeof(depthStencilEnabledDesc));
		depthStencilEnabledDesc.DepthEnable = true;
		depthStencilEnabledDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilEnabledDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilEnabledDesc.StencilEnable = true;
		depthStencilEnabledDesc.StencilReadMask = 0xFF;
		depthStencilEnabledDesc.StencilWriteMask = 0xFF;
		depthStencilEnabledDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilEnabledDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilEnabledDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilEnabledDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilEnabledDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilEnabledDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilEnabledDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilEnabledDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		result = DEVICE->CreateDepthStencilState(&depthStencilEnabledDesc, &depthStencilState);
		CHECK_RESULT(result, "Failed to create Depth Stencil State.");
		myDepthStencilStates[static_cast<int>(eDepthStencilState::eDefault)] = depthStencilState;
	}

	{
		depthStencilState = nullptr;
		D3D11_DEPTH_STENCIL_DESC depthStencilDisabledDesc;
		ZeroMemory(&depthStencilDisabledDesc, sizeof(depthStencilDisabledDesc));
		depthStencilDisabledDesc.DepthEnable = FALSE;
		depthStencilDisabledDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDisabledDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilDisabledDesc.StencilEnable = FALSE;
		depthStencilDisabledDesc.StencilReadMask = 0xFF;
		depthStencilDisabledDesc.StencilWriteMask = 0xFF;
		depthStencilDisabledDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDisabledDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilDisabledDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDisabledDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilDisabledDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDisabledDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilDisabledDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilDisabledDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		result = DEVICE->CreateDepthStencilState(&depthStencilDisabledDesc, &depthStencilState);
		CHECK_RESULT(result, "Failed to create Depth Stencil State.");
		myDepthStencilStates[static_cast<int>(eDepthStencilState::eDisableDepth)] = depthStencilState;
	}

	//tabort? ta den vaek
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilReadOnlyDesc;
		ZeroMemory(&depthStencilReadOnlyDesc, sizeof(depthStencilReadOnlyDesc));
		depthStencilReadOnlyDesc.DepthEnable = true;
		depthStencilReadOnlyDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		depthStencilReadOnlyDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilReadOnlyDesc.StencilEnable = true;
		depthStencilReadOnlyDesc.StencilReadMask = D3D11_DEFAULT_STENCIL_READ_MASK;
		depthStencilReadOnlyDesc.StencilWriteMask = D3D11_DEFAULT_STENCIL_WRITE_MASK;
		depthStencilReadOnlyDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilReadOnlyDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilReadOnlyDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilReadOnlyDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilReadOnlyDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilReadOnlyDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilReadOnlyDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilReadOnlyDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		result = DEVICE->CreateDepthStencilState(&depthStencilReadOnlyDesc, &depthStencilState);
		CHECK_RESULT(result, "Failed to create Depth Stencil State.");
		myDepthStencilStates[static_cast<int>(eDepthStencilState::eReadOnly)] = depthStencilState;
	}
}

void CRenderer::CreateSamplerStates()
{
	HRESULT result;
	ID3D11SamplerState* clampState = nullptr;
	ID3D11SamplerState* wrapState = nullptr;



	D3D11_SAMPLER_DESC samplerStateDesc;
	ZeroMemory(&samplerStateDesc, sizeof(samplerStateDesc));
	samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerStateDesc.MipLODBias = 0.0f;
	samplerStateDesc.MaxAnisotropy = 1;
	samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerStateDesc.BorderColor[0] = 0;
	samplerStateDesc.BorderColor[1] = 0;
	samplerStateDesc.BorderColor[2] = 0;
	samplerStateDesc.BorderColor[3] = 0;
	samplerStateDesc.MinLOD = -D3D11_FLOAT32_MAX;
	samplerStateDesc.MaxLOD = D3D11_FLOAT32_MAX;

	result = DEVICE->CreateSamplerState(&samplerStateDesc, &wrapState);
	mySamplerStates[static_cast<int>(eSamplerState::eWrap)] = wrapState;

	samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;


	result = DEVICE->CreateSamplerState(&samplerStateDesc, &clampState);
	mySamplerStates[static_cast<int>(eSamplerState::eClamp)] = clampState;

	mySamplerStates[static_cast<int>(eSamplerState::eClamp0Wrap1)] = nullptr;

	CHECK_RESULT(result, "Failed to create SamplerState.");
}

void CRenderer::DoRenderQueue()
{
	mySynchronizer.SwapRead();
	int drawCalls = 0;

	for (CSynchronizer<SRenderMessage*>::size_type i = 0; i < !mySynchronizer; ++i)
	{
		SRenderMessage* renderMessage = mySynchronizer[i];
		if (renderMessage == nullptr)
		{
			break;
		}
		HandleRenderMessage(renderMessage, drawCalls);
	}

	PostMaster::GetInstance().SendLetter(Message(eMessageType::eDrawCallsThisFrame, DrawCallsCount(drawCalls)));
}

void CRenderer::SetStates(const SChangeStatesMessage* aState) //change from pekare to reference
{
	DEVICE_CONTEXT->RSSetState(myRasterizerStates[static_cast<int>(aState->myRasterizerState)]);
	DEVICE_CONTEXT->OMSetDepthStencilState(myDepthStencilStates[static_cast<int>(aState->myDepthStencilState)], 0);
	

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;
	DEVICE_CONTEXT->OMSetBlendState(myBlendStates[static_cast<int>(aState->myBlendState)], blendFactor, sampleMask); // wha?


	assert(aState->mySamplerState != eSamplerState::eSize);
	//if (aState->mySamplerState == eSamplerState::eClamp0Wrap1)
	{
		ID3D11SamplerState* both[2] = { mySamplerStates[static_cast<int>(eSamplerState::eClamp)], mySamplerStates[static_cast<int>(eSamplerState::eWrap)] };
		DEVICE_CONTEXT->VSSetSamplers(0, 2, both);
		DEVICE_CONTEXT->PSSetSamplers(0, 2, both);
	}
	//else
	//{
	//	DEVICE_CONTEXT->VSSetSamplers(0, 1, &mySamplerStates[static_cast<int>(aState->mySamplerState)]);
	//	DEVICE_CONTEXT->PSSetSamplers(0, 1, &mySamplerStates[static_cast<int>(aState->mySamplerState)]);
	//}
}

void CRenderer::SetLights(CU::GrowingArray<CPointLightInstance>& aGrowings)
{
	myLightSynchronizer << &aGrowings;
}

void CRenderer::HandleRenderMessage(SRenderMessage * aRenderMesage, int & aDrawCallCount)
{
	switch (aRenderMesage->myType)
	{
	case SRenderMessage::eRenderMessageType::eActivateRenderTo:
	{
		//renderTo->Activate();
		gBuffer.SetBuffer();
		break;
	}
	case SRenderMessage::eRenderMessageType::eClearRenderPackage:
	{
		SClearRenderPackageMessage* msg = static_cast<SClearRenderPackageMessage*>(aRenderMesage);
		msg->myRenderPackage.Clear();
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderNavMesh:
	{
		SRenderNavmeshMessage* msg = static_cast<SRenderNavmeshMessage*>(aRenderMesage);
		CModel* model = msg->myModel;
		SRenderModelParams params;
		params.myTransform = msg->myTransformation;
		params.myTransformLastFrame = msg->myTransformation;
		
		params.myRenderToDepth = false;


		model->Render(params);
		++aDrawCallCount;
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderCameraQueue:
	{
		SRenderCameraQueueMessage* msg = static_cast<SRenderCameraQueueMessage*>(aRenderMesage);
		CU::Camera previousCam = myCamera;;
		myCamera = msg->myCamera;
		UpdateBuffer();

		msg->CameraRenderPackage.Clear();
		msg->CameraRenderPackage.Activate();
		for (unsigned int i = 0; i < msg->CameraRenderQueue.Size(); ++i)
		{
			HandleRenderMessage(msg->CameraRenderQueue[i], aDrawCallCount);
		}
		//msg->CameraRenderQueue.DeleteAll();
		myCamera = previousCam;
		gBuffer.SetBuffer();
		UpdateBuffer();
		break;
	}
	case SRenderMessage::eRenderMessageType::eSetShadowBuffer:
	{
		SSetShadowBuffer* msg = static_cast<SSetShadowBuffer*>(aRenderMesage);
		UpdateBuffer(msg);
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderModel:
	{
		SRenderModelMessage* msg = static_cast<SRenderModelMessage*>(aRenderMesage);
		CModelManager* modelManager = CEngine::GetInstance()->GetModelManager();
		if (!modelManager) break;

		CModel* model = modelManager->GetModel(msg->myModelID);
		if (!model) break;

		model->Render(msg->myRenderParams);
		++aDrawCallCount;
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderModelDepth:
	{
		SRenderModelDepthMessage* msg = static_cast<SRenderModelDepthMessage*>(aRenderMesage);
		CModel* model = CEngine::GetInstance()->GetModelManager()->GetModel(msg->myModelID);
		if (!model) break;
		model->Render(msg->myRenderParams);
		++aDrawCallCount;
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderGUIModel:
	{
		myGUIData.myInputPackage.Activate();
		SRenderGUIModelMessage* msg = static_cast<SRenderGUIModelMessage*>(aRenderMesage);
		CModelManager* modelManager = CEngine::GetInstance()->GetModelManager();
		CModel* model = modelManager->GetModel(msg->myModelID);

		if (model == nullptr) break;

		SRenderModelParams params;
		params.myTransform = msg->myToWorld;
		params.myTransformLastFrame = msg->myToWorld; //don't blur  GUI, atm fullösning deluxe.
		params.myRenderToDepth = false;
		params.aAnimationState = nullptr;

		if (model->HasConstantBuffer(CModel::eShaderStage::ePixel) == true)
		{
			msg->myPixelConstantBufferStruct.myCameraPosition = myCamera.GetPosition();
			model->UpdateConstantBuffer(CModel::eShaderStage::ePixel, &msg->myPixelConstantBufferStruct, sizeof(msg->myPixelConstantBufferStruct));
		}

		model->Render(params);

		++aDrawCallCount;


		gBuffer.SetBuffer();
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderSprite:
	{
		myGUIData.myInputPackage.Activate();
		SRenderSpriteMessage* msg = static_cast<SRenderSpriteMessage*>(aRenderMesage);
		msg->mySprite->Render(msg->myPosition, msg->mySize, msg->myPivot, msg->myRect, msg->myColor);
		++aDrawCallCount;

		//if (mySettings.Motionblur == true) renderTo->Activate(myMotionBlurData.velocityPackage);
		//else renderTo->Activate();
		gBuffer.SetBuffer();
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderText:
	{
		myGUIData.myInputPackage.Activate();
		SRenderTextMessage* msg = static_cast<SRenderTextMessage*>(aRenderMesage);
		msg->myText->Render(msg->myStrings, msg->myPosition, msg->myColor, msg->myAlignement);
		aDrawCallCount += msg->myStrings.Size();

		//if (mySettings.Motionblur == true) renderTo->Activate(myMotionBlurData.velocityPackage);
		//else renderTo->Activate();
		gBuffer.SetBuffer();
		break;
	}
	case SRenderMessage::eRenderMessageType::eChangeStates:
	{
		SChangeStatesMessage* msg = static_cast<SChangeStatesMessage*>(aRenderMesage);
		SetStates(msg);
		break;
	}
	case SRenderMessage::eRenderMessageType::eSetCamera:
	{
		SSetCameraMessage* msg = static_cast<SSetCameraMessage*>(aRenderMesage);
		myCamera = msg->myCamera;
		UpdateBuffer();
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderSkybox:
	{
		SRenderSkyboxMessage* msg = static_cast<SRenderSkyboxMessage*>(aRenderMesage);
		msg->mySkybox->Render(myCamera);
		++aDrawCallCount;
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderParticles:
	{
		SRenderParticlesMessage* msg = static_cast<SRenderParticlesMessage*>(aRenderMesage);
		CParticleEmitter* emitter = ENGINE->GetParticleEmitterManager().GetParticleEmitter(msg->particleEmitter);
		if (emitter == nullptr)	break;

		emitter->Render(msg->toWorld, msg->particleList);
		break;
	}
	case SRenderMessage::eRenderMessageType::eActivateRenderPackage:
	{
		SActivateRenderPackageMessage* msg = static_cast<SActivateRenderPackageMessage*>(aRenderMesage);
		if (msg->useSecondPackage == false)
		{
			msg->myRenderPackage.Activate();
		}
		else
		{
			msg->myRenderPackage.Activate(msg->mySecondRenderPackage);
		}
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderToInterediate:
	{
		SRenderToIntermediate* msg = static_cast<SRenderToIntermediate*>(aRenderMesage);
		myIntermediatePackage.Activate();
		myFullScreenHelper.DoEffect(
			CFullScreenHelper::eEffectType::eCopy, 
			msg->myRect, 
			msg->useDepthResource ? msg->myRenderPackage.GetDepthResource() : msg->myRenderPackage.GetResource());
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderFullscreenEffect:
	{
		SRenderFullscreenEffectMessage* msg = static_cast<SRenderFullscreenEffectMessage*>(aRenderMesage);

		bool c1 = msg->myFirstUseDepthResource;
		bool c2 = msg->mySecondUseDepthResource;

		CRenderPackage& p1  = msg->myFirstPackage;
		CRenderPackage& p2  = msg->mySecondPackage;

		myFullScreenHelper.DoEffect(
			msg->myEffectType,
			msg->myRect,
			c1 ? p1.GetDepthResource() : p1.GetResource(), 
			c2 ? p2.GetDepthResource() : p2.GetResource());

		++aDrawCallCount;
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderStreak:
	{
		SRenderStreakMessage* msg = static_cast<SRenderStreakMessage*>(aRenderMesage);
		msg->streakEmitter->Render();
		if (msg->streakEmitter->IsVisible() == true)
		{
			++aDrawCallCount;
		}
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderFire:
	{
		SRenderFireMessage* msg = static_cast<SRenderFireMessage*>(aRenderMesage);
		CFireEmitter* emitter = ENGINE->GetFireEmitterManager().GetFireEmitter(msg->myFireID);
		if (emitter == nullptr) break;

		const CU::Timer& fireTimer = myTimers.GetTimer(myFireTimer);
		if (fireTimer.GetLifeTime().GetMilliseconds() > 10000.f)
		{
			myTimers.ResetTimer(myFireTimer);
		}

		emitter->Render(fireTimer.GetLifeTime(), msg->myToWorldMatrix);
		++aDrawCallCount;
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderLineBuffer:
	{
		myGUIData.myInputPackage.Activate();
		SRenderLineBuffer* msg = static_cast<SRenderLineBuffer*>(aRenderMesage);
		CLineDrawer& lineDrawer = ENGINE->GetLineDrawer();
		lineDrawer.RenderLineChunk(msg->myLineBuffer);
		++aDrawCallCount;

		/*if (mySettings.Motionblur == true) renderTo->Activate(myMotionBlurData.velocityPackage);
		else renderTo->Activate();*/
		gBuffer.SetBuffer();
		break;
	}
	}
}

eMessageReturn CRenderer::Recieve(const Message & aMessage)
{
	return aMessage.myEvent.DoEvent(this);
}

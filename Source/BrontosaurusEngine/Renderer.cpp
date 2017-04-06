#include "stdafx.h"
#include "Renderer.h"
#include "DXFramework.h"
#include "Engine.h"
#include "PointLightInstance.h"
#include "Lights.h"
#include "Model.h"
#include "Text.h"
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
#include "../ThreadedPostmaster/Postmaster.h"
#include "../ThreadedPostmaster/PostOffice.h"
#include "TextureManager.h"
#include "Texture.h"

#define HDR_FORMAT DXGI_FORMAT_R32G32B32A32_FLOAT

CRenderer::CRenderer() : myParticleRenderer(*this, myFullScreenHelper)
{
	myIsRunning = true;

	mySettings.HDR = false;
	mySettings.Bloom = false;
	mySettings.Motionblur = false;
	mySettings.CromaticAberration = true;
	mySettings.FXAA = true;

	myOncePerFrameBufferTimer = myTimers.CreateTimer();
	myFireTimer = myTimers.CreateTimer();
	CreateOncePerFrameBuffer();
	CreateShadowBuffer();

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

	myLut = &TEXTUREMGR.LoadTexture("Lut/defaultLUT.dds");
	CU::Vector2ui windowSize = ENGINE->GetWindow()->GetWindowSize();
	myColorGradingPackage.Init(windowSize);


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
}

void CRenderer::Shutdown()
{
	myIsRunning = false;
}

void CRenderer::AddRenderMessage(SRenderMessage* aRenderMessage)
{
	mySynchronizer << aRenderMessage;
}

void CRenderer::Render()
{
	myTimers.UpdateTimers();
	UpdateBuffer();

	myBackBufferPackage.Clear();
	myIntermediatePackage.Clear();
	myGUIRenderer.GetInputPackage().Clear();
	myAntialiasingPackage.Clear();

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

	DoRenderQueue();

	//SChangeStatesMessage changeStateMessage = {};
	//changeStateMessage.myRasterizerState = eRasterizerState::eDefault;
	//changeStateMessage.myDepthStencilState = eDepthStencilState::eDefault;
	//changeStateMessage.myBlendState = eBlendState::eNoBlend;
	//changeStateMessage.mySamplerState = eSamplerState::eClamp;
	//SetStates(&changeStateMessage);
	//myDeferredRenderer.DoRenderQueue(*this);
	//changeStateMessage.myRasterizerState = eRasterizerState::eDefault;
	//changeStateMessage.myDepthStencilState = eDepthStencilState::eDefault;
	//changeStateMessage.myBlendState = eBlendState::eAlphaBlend;
	//changeStateMessage.mySamplerState = eSamplerState::eClamp;
	//SetStates(&changeStateMessage);
	//myDeferredRenderer.UpdateCameraBuffer(myCamera.GetTransformation(), myCamera.GetProjectionInverse());
	//myParticleRenderer.DoRenderQueue(myDeferredRenderer.GetDepthStencil(), myDeferredRenderer.GetDepthResource());
	//myDeferredRenderer.DoLightingPass(myFullScreenHelper, *this);
	//changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
	//changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
	//changeStateMessage.myBlendState = eBlendState::eAlphaBlend;
	//changeStateMessage.mySamplerState = eSamplerState::eClamp;
	//SetStates(&changeStateMessage);

	renderTo->Activate();
	
	Downsample(*renderTo);
	//HDR();
	Bloom();
	MotionBlur();
	//LensDistortion(myIntermediatePackage);

	AntiAliasing();
	DoColorGrading();
	myIntermediatePackage.Activate();
	myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myColorGradingPackage);

	int blup;//temp
	myGUIRenderer.DoRenderQueues(*this, blup);

	SChangeStatesMessage changeStateMessage;
	changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
	changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
	changeStateMessage.myBlendState = eBlendState::eAlphaBlend;
	changeStateMessage.mySamplerState = eSamplerState::eClamp;
	SetStates(&changeStateMessage);
	myGUIRenderer.RenderWholeGuiToPackage(myGUIRenderer.GetInputPackage(), myFullScreenHelper);
	RenderGUI();
	
	myBackBufferPackage.Activate();
	myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myIntermediatePackage);

	ID3D11ShaderResourceView* srvs[2] =
	{
		nullptr,
		nullptr
	};

	DEVICE_CONTEXT->PSSetShaderResources(0u, 2u, srvs);
}

void CRenderer::SwapWrite()
{
	mySynchronizer.SwapWrite();
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

void CRenderer::AntiAliasing()
{
	if (mySettings.FXAA == true)
	{
		myAntialiasingPackage.Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eAA, &myIntermediatePackage);
		ID3D11ShaderResourceView* srvs[2] =
		{
			nullptr,
			nullptr
		};

		DEVICE_CONTEXT->PSSetShaderResources(0u, 2u, srvs);

		myIntermediatePackage.Activate();
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myAntialiasingPackage);
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

	myIntermediatePackage.Activate();
	myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGUIRenderer.GetInputPackage());

	ID3D11ShaderResourceView* srvs [2] = 
	{
		nullptr,
		nullptr
	};

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

	myAntialiasingPackage.Init(windowSize);

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
		myGUIRenderer.GetInputPackage().Init(windowSize);
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
}

void CRenderer::CreateOncePerFrameBuffer()
{
	SOncePerFrameBuffer buffer;
	buffer.myCameraMatrices.myCameraSpaceInverse = myCamera.GetInverse();
	buffer.myCameraMatrices.myProjectionSpace = myCamera.GetProjection();
	buffer.myShadowCameraMatrices.myCameraSpaceInverse = CU::Matrix44f::Zero;
	buffer.myShadowCameraMatrices.myProjectionSpace = CU::Matrix44f::Zero;

	buffer.deltaTime = 0.0f;

	buffer.time = 0.0f;
	buffer.deltaTime = 0.0f;

	buffer.fogStart = 0.0f;
	buffer.fogEnd = 0.0f;

	myOncePerFrameBuffer = BSR::CreateCBuffer<SOncePerFrameBuffer>(&buffer);
}

void CRenderer::CreateShadowBuffer()
{
	SCascadeBuffer buffer;
	myShadowBuffer = BSR::CreateCBuffer<>(&buffer);
}

void CRenderer::UpdateBuffer()
{
	D3D11_MAPPED_SUBRESOURCE mappedSubResource;
	ZeroMemory(&mappedSubResource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	SOncePerFrameBuffer updatedBuffer;

	updatedBuffer.myCameraMatrices.myCameraSpaceInverse = myCamera.GetInverse();
	updatedBuffer.myCameraMatrices.myProjectionSpace = myCamera.GetProjection();

	updatedBuffer.myShadowCameraMatrices.myCameraSpaceInverse = myCamera.GetInverse();
	updatedBuffer.myShadowCameraMatrices.myProjectionSpace = myCamera.GetProjection();

	updatedBuffer.deltaTime = myTimers.GetTimer(myOncePerFrameBufferTimer).GetDeltaTime().GetSeconds();
	updatedBuffer.time = myTimers.GetTimer(myOncePerFrameBufferTimer).GetLifeTime().GetSeconds();
	updatedBuffer.fogStart = 0.0f;
	updatedBuffer.fogEnd = 0.0f;

	updatedBuffer.windowSize = CEngine::GetInstance()->GetWindowSize();

	DEVICE_CONTEXT->Map(myOncePerFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);

	memcpy(mappedSubResource.pData, &updatedBuffer, sizeof(SOncePerFrameBuffer));
	DEVICE_CONTEXT->Unmap(myOncePerFrameBuffer, 0);
	DEVICE_CONTEXT->VSSetConstantBuffers(0, 1, &myOncePerFrameBuffer);
	DEVICE_CONTEXT->GSSetConstantBuffers(0, 1, &myOncePerFrameBuffer);
	DEVICE_CONTEXT->PSSetConstantBuffers(0, 1, &myOncePerFrameBuffer);

}

void CRenderer::UpdateShadowBuffer(SSetShadowBuffer* msg)
{
	BSR::UpdateCBuffer<SCascadeBuffer>(myShadowBuffer, &msg->cascadeBuffer);

	DEVICE_CONTEXT->PSSetShaderResources(8, 1, &msg->myShadowBuffer.GetResource());
	DEVICE_CONTEXT->PSSetConstantBuffers(4, 1, &myShadowBuffer);
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


	ID3D11RasterizerState* FrontCullState = nullptr;
	D3D11_RASTERIZER_DESC rasterFrontFace;
	ZeroMemory(&rasterFrontFace, sizeof(rasterFrontFace));
	rasterFrontFace.FillMode = D3D11_FILL_SOLID;
	rasterFrontFace.CullMode = D3D11_CULL_FRONT;
	rasterFrontFace.DepthClipEnable = false;
	rasterFrontFace.FrontCounterClockwise = false;
	result = DEVICE->CreateRasterizerState(&rasterFrontFace, &FrontCullState);
	CHECK_RESULT(result, "Failed to create Rasterizer Default State.");
	myRasterizerStates[static_cast<int>(eRasterizerState::eCullFront)] = FrontCullState;



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
		blendDesc_AlphaBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		blendDesc_AlphaBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
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




	blendState = NULL;
	{
		D3D11_BLEND_DESC blendDesc_Blend = {};

		blendDesc_Blend.AlphaToCoverageEnable = FALSE;
		blendDesc_Blend.IndependentBlendEnable = TRUE;


		//blendDesc_Blend.RenderTarget[0].BlendEnable = TRUE;
		//blendDesc_Blend.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_INV_DEST_COLOR;
		//blendDesc_Blend.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_COLOR;
		//blendDesc_Blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		//blendDesc_Blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		//blendDesc_Blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		//blendDesc_Blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		//blendDesc_Blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		blendDesc_Blend.RenderTarget[0].BlendEnable = TRUE;
		blendDesc_Blend.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_SRC_COLOR;
		blendDesc_Blend.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_INV_SRC_COLOR;
		blendDesc_Blend.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		blendDesc_Blend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ONE;
		blendDesc_Blend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		blendDesc_Blend.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		blendDesc_Blend.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		result = DEVICE->CreateBlendState(&blendDesc_Blend, &blendState);
		CHECK_RESULT(result, "Failed to create add-Blend State.");
		myBlendStates[static_cast<int>(eBlendState::eAddBlend)] = blendState;
	}

	blendState = NULL;
	{
		D3D11_BLEND_DESC blendDesc_mul = {};

		blendDesc_mul.AlphaToCoverageEnable = FALSE;
		blendDesc_mul.IndependentBlendEnable = TRUE;

		blendDesc_mul.RenderTarget[0].BlendEnable = TRUE;
		blendDesc_mul.RenderTarget[0].SrcBlend = D3D11_BLEND::D3D11_BLEND_ZERO;
		blendDesc_mul.RenderTarget[0].DestBlend = D3D11_BLEND::D3D11_BLEND_SRC_COLOR;
		blendDesc_mul.RenderTarget[0].BlendOp = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;

		blendDesc_mul.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND::D3D11_BLEND_ZERO;
		blendDesc_mul.RenderTarget[0].DestBlendAlpha = D3D11_BLEND::D3D11_BLEND_SRC_ALPHA;
		blendDesc_mul.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP::D3D11_BLEND_OP_ADD;
		blendDesc_mul.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE::D3D11_COLOR_WRITE_ENABLE_ALL;

		result = DEVICE->CreateBlendState(&blendDesc_mul, &blendState);
		CHECK_RESULT(result, "Failed to create mul-Blend State.");
		myBlendStates[static_cast<int>(eBlendState::eMulBlend)] = blendState;
	}
	{
		D3D11_BLEND_DESC blendDesc_AlphaBlend = {};

		blendDesc_AlphaBlend.RenderTarget[0].BlendEnable = TRUE;
		blendDesc_AlphaBlend.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
		blendDesc_AlphaBlend.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
		blendDesc_AlphaBlend.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc_AlphaBlend.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_SRC_ALPHA;
		blendDesc_AlphaBlend.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_DEST_ALPHA;
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
		myBlendStates[static_cast<int>(eBlendState::eOverlay)] = blendState;
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
		depthStencilEnabledDesc.StencilEnable = FALSE;
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

	// Lesser!
	{
		D3D11_DEPTH_STENCIL_DESC depthStencilEnabledDesc;
		ZeroMemory(&depthStencilEnabledDesc, sizeof(depthStencilEnabledDesc));
		depthStencilEnabledDesc.DepthEnable = true;
		depthStencilEnabledDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilEnabledDesc.DepthFunc = D3D11_COMPARISON_LESS;
		depthStencilEnabledDesc.StencilEnable = false;
		depthStencilEnabledDesc.StencilReadMask = 0xFF;
		depthStencilEnabledDesc.StencilWriteMask = 0xFF;
		depthStencilEnabledDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilEnabledDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
		depthStencilEnabledDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilEnabledDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
		depthStencilEnabledDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
		depthStencilEnabledDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
		depthStencilEnabledDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
		depthStencilEnabledDesc.BackFace.StencilFunc = D3D11_COMPARISON_NEVER;
		result = DEVICE->CreateDepthStencilState(&depthStencilEnabledDesc, &depthStencilState);
		CHECK_RESULT(result, "Failed to create Depth Stencil State.");
		myDepthStencilStates[static_cast<int>(eDepthStencilState::eLesser)] = depthStencilState;
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
	ID3D11SamplerState* pointWrapState = nullptr;


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

	samplerStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
	samplerStateDesc.ComparisonFunc = D3D11_COMPARISON_LESS_EQUAL;

	result = DEVICE->CreateSamplerState(&samplerStateDesc, &pointWrapState);
	mySamplerStates[static_cast<int>(eSamplerState::eClamp0Wrap1)] = pointWrapState;

	CHECK_RESULT(result, "Failed to create SamplerStates.");



	ID3D11SamplerState* troth[3] =
	{
		mySamplerStates[static_cast<int>(eSamplerState::eClamp)],
		mySamplerStates[static_cast<int>(eSamplerState::eWrap)],
		mySamplerStates[static_cast<int>(eSamplerState::eClamp0Wrap1)]
	};
	DEVICE_CONTEXT->VSSetSamplers(0, 3, troth);
	DEVICE_CONTEXT->PSSetSamplers(0, 3, troth);

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
	//PostMaster::GetInstance().SendLetter(Message(eMessageType::eDrawCallsThisFrame, DrawCallsCount(drawCalls)));
	Postmaster::Threaded::CPostmaster::GetInstance().Broadcast(new DrawCallsCount(drawCalls));
	Postmaster::Threaded::CPostmaster::GetInstance().GetThreadOffice().HandleMessages();
}

void CRenderer::SetStates(const SChangeStatesMessage* aState) //change from pekare to reference
{
	DEVICE_CONTEXT->RSSetState(myRasterizerStates[static_cast<int>(aState->myRasterizerState)]);
	DEVICE_CONTEXT->OMSetDepthStencilState(myDepthStencilStates[static_cast<int>(aState->myDepthStencilState)], 0);

	float blendFactor[4] = { 0.0f, 0.0f, 0.0f, 0.0f };
	UINT sampleMask = 0xffffffff;
	DEVICE_CONTEXT->OMSetBlendState(myBlendStates[static_cast<int>(aState->myBlendState)], blendFactor, sampleMask); // wha?


	assert(aState->mySamplerState != eSamplerState::eSize);
	//else
	//{
	//	DEVICE_CONTEXT->VSSetSamplers(0, 1, &mySamplerStates[static_cast<int>(aState->mySamplerState)]);
	//	DEVICE_CONTEXT->PSSetSamplers(0, 1, &mySamplerStates[static_cast<int>(aState->mySamplerState)]);
	//}
}

const CU::Camera& CRenderer::GetCamera()
{
	return myCamera;
}

void CRenderer::HandleRenderMessage(SRenderMessage * aRenderMesage, int & aDrawCallCount)
{
	switch (aRenderMesage->myType)
	{
	case SRenderMessage::eRenderMessageType::eRenderPointLight:
	case SRenderMessage::eRenderMessageType::eRenderSpotLight:
	case SRenderMessage::eRenderMessageType::eRenderDirectionalLight:
	{
		myDeferredRenderer.AddRenderMessage(aRenderMesage);
		myParticleRenderer.AddRenderMessage(aRenderMesage);
		//++aDrawCallCount;
		break;
	}
	case SRenderMessage::eRenderMessageType::eSetCubemapResource:
	{
		SSetCubemapResource* msg = static_cast<SSetCubemapResource*>(aRenderMesage);
		DEVICE_CONTEXT->PSSetShaderResources(0, 1, &msg->mySRV);
		break;
	}
	case SRenderMessage::eRenderMessageType::eClear:
	{
		SClear* msg = static_cast<SClear*>(aRenderMesage);
		float r, g, b = g = r = 1.0f;
		float clearColour[4] = { r, g, b, 1.0f };
		DEVICE_CONTEXT->ClearRenderTargetView(msg->myRTV, clearColour);
		break;
	}
	case SRenderMessage::eRenderMessageType::eActivateRenderTo:
	{
		renderTo->Activate();
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
		SForwardRenderModelParams params;
		params.myTransform = msg->myTransformation;
		params.myTransformLastFrame = msg->myTransformation;

		params.myRenderToDepth = false;


		model->Render(params);
		++aDrawCallCount;
		break;
	}
	case SRenderMessage::eRenderMessageType::eSetRTV:
	{
		SSetRTVMessage* msg = static_cast<SSetRTVMessage*>(aRenderMesage);
		float r, g, b = g = r = 1.0f;
		float clearColour[4] = { r, g, b, 1.0f };
		DEVICE_CONTEXT->ClearRenderTargetView(msg->myRTV, clearColour);
		DEVICE_CONTEXT->RSSetViewports(1, msg->myViewport);
		DEVICE_CONTEXT->OMSetRenderTargets(1, &msg->myRTV, nullptr);
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderCameraQueue:
	{
		SRenderCameraQueueMessage* msg = static_cast<SRenderCameraQueueMessage*>(aRenderMesage);
		RenderCameraQueue(msg, aDrawCallCount);
		break;
	}
	case SRenderMessage::eRenderMessageType::eSetShadowBuffer:
	{
		SSetShadowBuffer* msg = static_cast<SSetShadowBuffer*>(aRenderMesage);
		UpdateShadowBuffer(msg);
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
	case SRenderMessage::eRenderMessageType::eRenderModelShadow:
	{
		SRenderModelShadowMessage* msg = static_cast<SRenderModelShadowMessage*>(aRenderMesage);
		CModel* model = CEngine::GetInstance()->GetModelManager()->GetModel(msg->myModelID);
		if (!model) break;
		model->Render(msg->myRenderParams);
		++aDrawCallCount;
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderModelDeferred:
	{
		myDeferredRenderer.AddRenderMessage(aRenderMesage);
		++aDrawCallCount;
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderModelInstanced:
	{
		myDeferredRenderer.AddRenderMessage(aRenderMesage);
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderModelBatches:
	{
		myDeferredRenderer.AddRenderMessage(aRenderMesage);
		aDrawCallCount += myDeferredRenderer.myBatchedModelIds.Size();
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderGUIModel:
	{
		myGUIRenderer.GetCurrentPackage().Activate();
		SRenderGUIModelMessage* msg = static_cast<SRenderGUIModelMessage*>(aRenderMesage);
		CModelManager* modelManager = CEngine::GetInstance()->GetModelManager();
		CModel* model = modelManager->GetModel(msg->myModelID);

		if (model == nullptr) break;

		SForwardRenderModelParams params;
		params.myTransform = msg->myToWorld;
		params.myTransformLastFrame = msg->myToWorld; //don't blur  GUI, atm fullösning deluxe.
		params.myRenderToDepth = false;

		model->Render(params);

		++aDrawCallCount;

		if (mySettings.Motionblur == true) renderTo->Activate(myMotionBlurData.velocityPackage);
		else renderTo->Activate();

		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderSprite:
	{
		myGUIRenderer.GetCurrentPackage().Activate();
		SRenderSpriteMessage* msg = static_cast<SRenderSpriteMessage*>(aRenderMesage);
		msg->mySprite->Render(msg->myPosition, msg->mySize, msg->myPivot, msg->myRect, msg->myColor);
		++aDrawCallCount;

		if (mySettings.Motionblur == true) renderTo->Activate(myMotionBlurData.velocityPackage);
		else renderTo->Activate();
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderText:
	{
		myGUIRenderer.GetCurrentPackage().Activate();
		SRenderTextMessage* msg = static_cast<SRenderTextMessage*>(aRenderMesage);
		msg->myText->Render(msg->myStrings, msg->myPosition, msg->myColor, msg->myAlignement, myGUIRenderer.GetCurrentPackage().GetSize());
#ifndef _RETAIL_BUILD
		for (std::wstring& row : msg->myStrings)
		{
			aDrawCallCount += static_cast<int>(row.size());
		}
#endif // !_RETAIL_BUILD

		if (mySettings.Motionblur == true) renderTo->Activate(myMotionBlurData.velocityPackage);
		else renderTo->Activate();
		break;
	}
	case SRenderMessage::eRenderMessageType::eChangeStates:
	{
		SChangeStatesMessage* msg = static_cast<SChangeStatesMessage*>(aRenderMesage);
		if(msg->mySamplerState == eSamplerState::eDeferred) myDeferredRenderer.AddRenderMessage(msg);
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
		myParticleRenderer.AddRenderMessage(static_cast<SRenderParticlesMessage*>(aRenderMesage));

		++aDrawCallCount;
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

		if (msg->unbindShadowBuffer == true)
		{
			ID3D11ShaderResourceView* srv = nullptr;
			DEVICE_CONTEXT->PSSetShaderResources(8, 1, &srv);
		}

		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderToInterediate:
	{
		SRenderToIntermediate* msg = static_cast<SRenderToIntermediate*>(aRenderMesage);
		myIntermediatePackage.Activate();

		myFullScreenHelper.DoEffect(
			msg->useDepthResource ? CFullScreenHelper::eEffectType::eCopyR : CFullScreenHelper::eEffectType::eCopy,
			msg->myRect,
			msg->useDepthResource ? msg->myRenderPackage.GetDepthResource() : msg->myRenderPackage.GetResource());

		++aDrawCallCount;
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderFullscreenEffect:
	{
		SRenderFullscreenEffectMessage* msg = static_cast<SRenderFullscreenEffectMessage*>(aRenderMesage);

		bool c1 = msg->myFirstUseDepthResource;
		bool c2 = msg->mySecondUseDepthResource;

		CRenderPackage& p1 = msg->myFirstPackage;
		CRenderPackage& p2 = msg->mySecondPackage;

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
		myGUIRenderer.GetCurrentPackage().Activate();
		SRenderLineBuffer* msg = static_cast<SRenderLineBuffer*>(aRenderMesage);
		CLineDrawer& lineDrawer = ENGINE->GetLineDrawer();
		lineDrawer.RenderLineChunk(msg->myLineBuffer);
		++aDrawCallCount;

		if (mySettings.Motionblur == true) renderTo->Activate(myMotionBlurData.velocityPackage);
		else renderTo->Activate();
		break;
	}
	case SRenderMessage::eRenderMessageType::eCreateGuiElement:
	{
		SCreateOrClearGuiElement* createOrClearGuiElement = static_cast<SCreateOrClearGuiElement*>(aRenderMesage);
		myGUIRenderer.CreateOrClearEvent(createOrClearGuiElement->myElementName, createOrClearGuiElement->myPixelSize, createOrClearGuiElement->myGuiElement);
		break;
	}
	case SRenderBarMessage::eRenderMessageType::eRenderToGui:
	{
		SRenderToGUI* renderToGuiMessage = static_cast<SRenderToGUI*>(aRenderMesage);
		myGUIRenderer.RenderToGUI(renderToGuiMessage->myElementName, renderToGuiMessage->myRenderMessage);
		break;
	}
	case SRenderMessage::eRenderMessageType::eRenderBar:
	{
		SRenderBarMessage* msg = static_cast<SRenderBarMessage*>(aRenderMesage);
		myGUIRenderer.GetCurrentPackage().Activate();

		myGUIRenderer.RenderBar(msg);

		if (mySettings.Motionblur == true) renderTo->Activate(myMotionBlurData.velocityPackage);
		else renderTo->Activate();
		break;
	}
	default: break;
	}
}

void CRenderer::RenderCameraQueue(SRenderCameraQueueMessage* msg, int & aDrawCallCount)
{
	myCamera = msg->myRenderCamera.GetCamera();
	UpdateBuffer();

	ID3D11DeviceContext* context = DEVICE_CONTEXT;
	msg->myRenderCamera.myRenderPackage.Clear();
	msg->myRenderCamera.myRenderPackage.Activate();

	for (unsigned int i = 0; i < msg->myRenderCamera.myRenderQueue.Size(); ++i)
	{
		HandleRenderMessage(msg->myRenderCamera.myRenderQueue[i], aDrawCallCount);
	}

	myDeferredRenderer.SetGeometryBuffer(*msg->myRenderCamera.myGbuffer);
	SChangeStatesMessage changeStateMessage = {};
	changeStateMessage.myRasterizerState = eRasterizerState::eDefault;
	changeStateMessage.myDepthStencilState = eDepthStencilState::eDefault;
	changeStateMessage.myBlendState = eBlendState::eNoBlend;
	changeStateMessage.mySamplerState = eSamplerState::eClamp;
	SetStates(&changeStateMessage);

	myDeferredRenderer.DoRenderQueue(*this);

	if (msg->myRenderCamera.GetIsShadowCamera() == false)
	{
		changeStateMessage.myRasterizerState = eRasterizerState::eDefault;
		changeStateMessage.myDepthStencilState = eDepthStencilState::eDefault;
		changeStateMessage.myBlendState = eBlendState::eAlphaBlend;
		changeStateMessage.mySamplerState = eSamplerState::eClamp;
		SetStates(&changeStateMessage);
		myDeferredRenderer.UpdateCameraBuffer(myCamera.GetTransformation(), myCamera.GetProjectionInverse());
		myParticleRenderer.DoRenderQueue(myDeferredRenderer.GetDepthStencil(), myDeferredRenderer.GetDepthResource());
		myDeferredRenderer.DoLightingPass(myFullScreenHelper, *this);
	}

	changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
	changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
	changeStateMessage.myBlendState = eBlendState::eAlphaBlend;
	changeStateMessage.mySamplerState = eSamplerState::eClamp;
	SetStates(&changeStateMessage);

	msg->myRenderCamera.GetRenderPackage().Activate();

	if (msg->myRenderCamera.GetIsShadowCamera() == false)
	{
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myDeferredRenderer.myIntermediatePackage);
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myParticleRenderer.GetIntermediatePackage());
	}
	else
	{
		myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, myDeferredRenderer.GetDepthResource());
	}
}

void CRenderer::DoColorGrading()
{
	myColorGradingPackage.Clear();
	myColorGradingPackage.Activate();
	DEVICE_CONTEXT->PSSetShaderResources(2, 1, myLut->GetShaderResourceViewPointer());
	myFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eColorGrading, &myIntermediatePackage);
}

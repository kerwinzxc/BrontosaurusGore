#include "stdafx.h"
#include "DeferredRenderer.h"
#include "RenderMessages.h"
#include "Engine.h"
#include "DXFramework.h"
#include "ModelManager.h"
#include "ConstBufferTemplate.h"
#include "FullScreenHelper.h"
#include "Lights.h"
#include "Renderer.h"
#include "LightModel.h"
#include "Engine.h"
#include "ShaderManager.h"
#include "FBXLoader.h"
#include "ParticleEmitterManager.h"
#include "ParticleEmitter.h"
#include "Texture.h"
#include "TextureManager.h"
#include "Engine.h"

#include <../CommonUtilities/InputWrapper.h>
#include "WindowsWindow.h"
#include "EKeyboardKeys.h"

CDeferredRenderer::CDeferredRenderer()
{
	myFramework = CEngine::GetInstance()->GetFramework();
	CU::Vector2ui windowSize = CEngine::GetInstance()->GetWindowSize();

	myGbuffer.diffuse.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	myGbuffer.normal.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	myGbuffer.emissive.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	myGbuffer.RMAO.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	myGbuffer.highLight.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);


	myIntermediatePackage.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);
	mySSAORandomTexture = &CEngine::GetInstance()->GetTextureManager().LoadTexture("SSAORandomTexture.dds");
	mySSAOPackage.Init(windowSize, nullptr, DXGI_FORMAT_R8G8B8A8_UNORM);

	myRenderMessages.Init(128);
	myLightMessages.Init(128);

	SCameraStruct temp;
	myProjectionInverseBuffer = BSR::CreateCBuffer<SCameraStruct>(&temp);

	Lights::SDirectionalLight directionalLight;
	myDirectionalLightBuffer = BSR::CreateCBuffer<Lights::SDirectionalLight>(&directionalLight);

	Lights::SPointLight pointLight;
	myPointLightBuffer = BSR::CreateCBuffer<Lights::SPointLight>(&pointLight);

	Lights::SSpotLight spotLight;
	mySpotLightBuffer = BSR::CreateCBuffer<Lights::SSpotLight>(&spotLight);

	InitPointLightModel();
	
	
#ifdef _ENABLE_RENDERMODES
	myInputWrapper = new CU::InputWrapper();
	myInputWrapper->Init(HINSTGET, HWNDGET);
	myRenderMode = ERenderMode::eIntermediate;
#endif
}

CDeferredRenderer::~CDeferredRenderer()
{
	SAFE_RELEASE(myProjectionInverseBuffer);
	SAFE_RELEASE(myDirectionalLightBuffer);
	SAFE_RELEASE(myPointLightBuffer);
	SAFE_RELEASE(mySpotLightBuffer);
	SAFE_DELETE(myLightModel);
}

void CDeferredRenderer::InitPointLightModel()
{
	myLightModel = new CLightModel();

	CFBXLoader modelLoader;
	CLoaderModel* model = modelLoader.LoadModel("Models/lightmeshes/sphere.fbx");
	CLoaderMesh* mesh = model->myMeshes[0];

	unsigned int shaderBlueprint = mesh->myShaderType;
	ID3D11VertexShader* vertexShader = SHADERMGR->LoadVertexShader(L"Shaders/Deferred/deferred_vertex.fx", shaderBlueprint);
	ID3D11PixelShader* pixelShader = SHADERMGR->LoadPixelShader(L"Shaders/Deferred/deferred_pointlight.fx", shaderBlueprint);
	ID3D11InputLayout* inputLayout = SHADERMGR->LoadInputLayout(L"Shaders/Deferred/deferred_vertex.fx", shaderBlueprint);


	CEffect* effect = new CEffect(vertexShader, pixelShader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myLightModel->Init(effect, mesh);
}

void CDeferredRenderer::DoRenderMessage(SRenderMessage* aRenderMessage)
{
	switch (aRenderMessage->myType)
	{
	case SRenderMessage::eRenderMessageType::eRenderModelDeferred:
	{
		SRenderModelDeferredMessage* msg = static_cast<SRenderModelDeferredMessage*>(aRenderMessage);
		CModelManager* modelManager = CEngine::GetInstance()->GetModelManager();
		if (!modelManager) break;
		CModel* model = modelManager->GetModel(msg->myModelID);
		if (!model) break;
		model->Render(msg->myRenderParams);
		break;
	}
	default:
		break;
	}
}

void CDeferredRenderer::DoRenderQueue()
{
	ClearRenderTargets();
	SetRenderTargets();


	for (unsigned int i = 0; i < myRenderMessages.Size(); ++i)
	{
		switch (myRenderMessages[i]->myType)
		{
		case SRenderMessage::eRenderMessageType::eRenderModelDeferred:
		{
			SRenderModelDeferredMessage* msg = static_cast<SRenderModelDeferredMessage*>(myRenderMessages[i]);
			CModelManager* modelManager = CEngine::GetInstance()->GetModelManager();
			if (!modelManager) break;

			CModel* model = modelManager->GetModel(msg->myModelID);
			if (!model) break;

			model->Render(msg->myRenderParams);
			break;
		}
		default:
			break;
		}
	}

	myRenderMessages.RemoveAll();
}

void CDeferredRenderer::AddRenderMessage(SRenderMessage* aRenderMessage)
{
	switch (aRenderMessage->myType)
	{
	case SRenderMessage::eRenderMessageType::eRenderSpotLight:
	case SRenderMessage::eRenderMessageType::eRenderPointLight:
	case SRenderMessage::eRenderMessageType::eRenderDirectionalLight:
		myLightMessages.Add(aRenderMessage);
		break;
	case SRenderMessage::eRenderMessageType::eRenderModelDeferred:
		myRenderMessages.Add(aRenderMessage);
		break;
	}
}

void CDeferredRenderer::UpdateCameraBuffer(const CU::Matrix44f & aCameraSpace, const CU::Matrix44f & aProjectionInverse)
{
	SCameraStruct updatedMatrices;
	updatedMatrices.prjInv = aProjectionInverse;
	updatedMatrices.camTW = aCameraSpace;

	BSR::UpdateCBuffer<SCameraStruct>(myProjectionInverseBuffer, &updatedMatrices);
}

void CDeferredRenderer::DoLightingPass(CFullScreenHelper& aFullscreenHelper, CRenderer& aRenderer)
{
	SetCBuffer();

	//SSAO
#ifdef _ENABLE_RENDERMODES
	if(myRenderMode == ERenderMode::eSSAO)
#endif
	{
		SChangeStatesMessage changeStateMessage = {};
		changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
		changeStateMessage.myDepthStencilState = eDepthStencilState::eDefault;
		changeStateMessage.myBlendState = eBlendState::eMulBlend;
		changeStateMessage.mySamplerState = eSamplerState::eClamp;
		aRenderer.SetStates(&changeStateMessage);


		ActivateSSAO();
		DoSSAO(aFullscreenHelper);
	}
	SChangeStatesMessage changeStateMessage = {};
	changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
	changeStateMessage.myDepthStencilState = eDepthStencilState::eDefault;
	changeStateMessage.myBlendState = eBlendState::eAlphaBlend;
	changeStateMessage.mySamplerState = eSamplerState::eClamp;
	aRenderer.SetStates(&changeStateMessage);


	ActivateIntermediate();
	SetSRV();
#ifdef _ENABLE_RENDERMODES
	myInputWrapper->Update();
	HandleInput();

	switch (myRenderMode)
	{
	case CDeferredRenderer::eDiffuse:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGbuffer.diffuse);
		break;
	case CDeferredRenderer::eNormal:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGbuffer.normal);
		break;
	case CDeferredRenderer::eRoughness:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopyR, &myGbuffer.RMAO);
		break;
	case CDeferredRenderer::eMetalness:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopyG, &myGbuffer.RMAO);
		break;
	case CDeferredRenderer::eAO:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopyB, &myGbuffer.RMAO);
		break;
	case CDeferredRenderer::eSSAO:
		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopyB, &myGbuffer.RMAO);
		break;

	case eHighlight: 

		aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGbuffer.highLight);
		break;
	case CDeferredRenderer::eIntermediate:
		changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
		changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStateMessage.myBlendState = eBlendState::eNoBlend;
		changeStateMessage.mySamplerState = eSamplerState::eClamp;
		aRenderer.SetStates(&changeStateMessage);
		DoAmbientLighting(aFullscreenHelper);
		changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
		changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStateMessage.myBlendState = eBlendState::eAddBlend;
		changeStateMessage.mySamplerState = eSamplerState::eClamp;
		aRenderer.SetStates(&changeStateMessage);
		DoDirectLighting(aFullscreenHelper);

		

		DoHighlight(aFullscreenHelper,aRenderer);
		break;
	default:
		break;
	}

#else
		changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
		changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStateMessage.myBlendState = eBlendState::eNoBlend;
		changeStateMessage.mySamplerState = eSamplerState::eClamp;
		aRenderer.SetStates(&changeStateMessage);
		DoAmbientLighting(aFullscreenHelper);
		changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
		changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
		changeStateMessage.myBlendState = eBlendState::eAddBlend;
		changeStateMessage.mySamplerState = eSamplerState::eClamp;
		aRenderer.SetStates(&changeStateMessage);
		DoDirectLighting(aFullscreenHelper);
#endif
}

ID3D11DepthStencilView* CDeferredRenderer::GetDepthStencil()
{
	return myGbuffer.diffuse.GetDepthStencilView();
}

ID3D11ShaderResourceView* CDeferredRenderer::GetDepthResource()
{
	return myGbuffer.diffuse.GetDepthResource();
}

void CDeferredRenderer::SetRenderTargets()
{
	ID3D11ShaderResourceView* srvs[5];
	srvs[0] = nullptr;
	srvs[1] = nullptr;
	srvs[2] = nullptr;
	srvs[3] = nullptr;
	srvs[4] = nullptr;

	myFramework->GetDeviceContext()->PSSetShaderResources(1, 5, srvs);


	ID3D11RenderTargetView* rtvs[5];
	rtvs[0] = myGbuffer.diffuse.GetRenderTargetView();
	rtvs[1] = myGbuffer.normal.GetRenderTargetView();
	rtvs[2] = myGbuffer.RMAO.GetRenderTargetView();
	rtvs[3] = myGbuffer.emissive.GetRenderTargetView();
	rtvs[4] = myGbuffer.highLight.GetRenderTargetView();

	myFramework->GetDeviceContext()->OMSetRenderTargets(5, rtvs, myGbuffer.diffuse.GetDepthStencilView());
}

void CDeferredRenderer::ClearRenderTargets()
{
	myGbuffer.diffuse.Clear();
	myGbuffer.normal.Clear();
	myGbuffer.RMAO.Clear();
	myGbuffer.emissive.Clear();
	myGbuffer.highLight.Clear();
}

void CDeferredRenderer::ActivateIntermediate()
{
	ID3D11RenderTargetView* rtvs[4];
	rtvs[0] = nullptr;
	rtvs[1] = nullptr;
	rtvs[2] = nullptr;
	rtvs[3] = nullptr;
	myFramework->GetDeviceContext()->OMSetRenderTargets(4, rtvs, nullptr);


	myIntermediatePackage.Clear();
	myIntermediatePackage.Activate();
}

void CDeferredRenderer::ActivateSSAO()
{
	ID3D11RenderTargetView* rtvs[4];
	rtvs[0] = nullptr;
	rtvs[1] = nullptr;
	rtvs[2] = nullptr;
	rtvs[3] = nullptr;
	myFramework->GetDeviceContext()->OMSetRenderTargets(4, rtvs, nullptr);

	myGbuffer.RMAO.Activate();
	//mySSAOPackage.Clear();
	//mySSAOPackage.Activate();

	ID3D11ShaderResourceView* srvs[5];
	srvs[0] = myGbuffer.diffuse.GetResource();
	srvs[1] = myGbuffer.normal.GetResource();
	srvs[2] = nullptr;//myGbuffer.RMAO.GetResource();
	srvs[3] = myGbuffer.emissive.GetResource();
	srvs[4] = myGbuffer.diffuse.GetDepthResource();
	myFramework->GetDeviceContext()->PSSetShaderResources(1, 5, srvs);
}

void CDeferredRenderer::SetSRV()
{
	ID3D11ShaderResourceView* srvs[5];
	srvs[0] = myGbuffer.diffuse.GetResource();
	srvs[1] = myGbuffer.normal.GetResource();
	srvs[2] = myGbuffer.RMAO.GetResource();
	srvs[3] = myGbuffer.emissive.GetResource();
	srvs[4] = myGbuffer.diffuse.GetDepthResource();
	myFramework->GetDeviceContext()->PSSetShaderResources(1, 5, srvs);
}

void CDeferredRenderer::SetRMAOSRV()
{
	myFramework->GetDeviceContext()->PSSetShaderResources(3, 1, &myGbuffer.RMAO.GetResource());
}

void CDeferredRenderer::SetCBuffer()
{
	myFramework->GetDeviceContext()->PSSetConstantBuffers(1, 1, &myProjectionInverseBuffer);
}

void CDeferredRenderer::DoAmbientLighting(CFullScreenHelper& aFullscreenHelper)
{
	aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eDeferredAmbient);
}

void CDeferredRenderer::DoDirectLighting(CFullScreenHelper& aFullscreenHelper)
{
	for (SRenderMessage* renderMessage : myLightMessages)
	{
		switch (renderMessage->myType)
		{
		case SRenderMessage::eRenderMessageType::eRenderDirectionalLight:
			RenderDirectionalLight(renderMessage, aFullscreenHelper);
			break;
		case SRenderMessage::eRenderMessageType::eRenderPointLight:
			RenderPointLight(renderMessage, aFullscreenHelper);
			break;
		case SRenderMessage::eRenderMessageType::eRenderSpotLight:
			RenderSpotLight(renderMessage, aFullscreenHelper);
			break;
		}
	}
	myLightMessages.RemoveAll();
}

void CDeferredRenderer::RenderDirectionalLight(SRenderMessage* aRenderMessage, CFullScreenHelper& aFullscreenHelper)
{
	SRenderDirectionalLight* msg = static_cast<SRenderDirectionalLight*>(aRenderMessage);
	BSR::UpdateCBuffer<Lights::SDirectionalLight>(myDirectionalLightBuffer, &msg->directionalLight);
	myFramework->GetDeviceContext()->PSSetConstantBuffers(2, 1, &myDirectionalLightBuffer);
	aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eDeferredDirectional);
}

void CDeferredRenderer::RenderPointLight(SRenderMessage* aRenderMessage, CFullScreenHelper& aFullscreenHelper)
{
	SRenderPointLight* msg = static_cast<SRenderPointLight*>(aRenderMessage);
	BSR::UpdateCBuffer<Lights::SPointLight>(myPointLightBuffer, &msg->pointLight);
	myFramework->GetDeviceContext()->PSSetConstantBuffers(2, 1, &myPointLightBuffer);
	CU::Matrix44f pointLightTransformation;
	pointLightTransformation.myPosition = msg->pointLight.position;
	float scale = msg->pointLight.range;
	pointLightTransformation.Scale({ scale, scale, scale });
	myLightModel->Render(pointLightTransformation);
}

void CDeferredRenderer::RenderSpotLight(SRenderMessage* aRenderMessage, CFullScreenHelper& aFullscreenHelper)
{
	SRenderSpotLight* msg = static_cast<SRenderSpotLight*>(aRenderMessage);
	BSR::UpdateCBuffer<Lights::SSpotLight>(mySpotLightBuffer, &msg->spotLight);
	myFramework->GetDeviceContext()->PSSetConstantBuffers(2, 1, &mySpotLightBuffer);
	aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eDeferredSpotLight);
}

void CDeferredRenderer::DoHighlight(CFullScreenHelper& aFullscreenHelper, CRenderer& aRenderer)
{
	SChangeStatesMessage changeStateMessage;
	/*changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
	changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
	changeStateMessage.myBlendState = eBlendState::eAlphaBlend;
	changeStateMessage.mySamplerState = eSamplerState::eClamp;
	aRenderer.SetStates(&changeStateMessage);
	myGbuffer.highLight.Activate();

	aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eGaussianBlurHorizontal, &myGbuffer.highLight);
	aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eGaussianBlurVertical, &myGbuffer.highLight);*/

	changeStateMessage.myRasterizerState = eRasterizerState::eNoCulling;
	changeStateMessage.myDepthStencilState = eDepthStencilState::eDisableDepth;
	changeStateMessage.myBlendState = eBlendState::eOverlay;
	changeStateMessage.mySamplerState = eSamplerState::eClamp;
	aRenderer.SetStates(&changeStateMessage);
	//ActivateIntermediate();
	aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myGbuffer.highLight);
}


void CDeferredRenderer::DoSSAO(CFullScreenHelper& aFullscreenHelper)
{
	myFramework->GetDeviceContext()->PSSetShaderResources(6, 1, mySSAORandomTexture->GetShaderResourceViewPointer());
	aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eSSAO);
}

#ifdef _ENABLE_RENDERMODES
void CDeferredRenderer::HandleInput()
{
	if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F1))
	{
		myRenderMode = ERenderMode::eIntermediate;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F1))
	{
		myRenderMode = ERenderMode::eDiffuse;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F2))
	{
		myRenderMode = ERenderMode::eNormal;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F3))
	{
		myRenderMode = ERenderMode::eRoughness;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F4))
	{
		myRenderMode = ERenderMode::eMetalness;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F5))
	{
		myRenderMode = ERenderMode::eAO;
	}
	else if (myInputWrapper->IsKeyboardKeyDown(CU::eKeys::F6))
	{
		myRenderMode = ERenderMode::eSSAO;
	}
}
#endif
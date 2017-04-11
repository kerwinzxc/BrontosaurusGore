#include "stdafx.h"
#include "FullScreenHelper.h"
#include "DXFramework.h"
#include "Effect.h"
#include "RenderPackage.h"
#include "WindowsWindow.h"
#include "ShaderManager.h"
#include "EModelBluePrint.h"

CFullScreenHelper::CFullScreenHelper()
{
	short ShaderType = EModelBluePrint::EModelBluePrint_Position | EModelBluePrint::EModelBluePrint_UV;

	ID3D11VertexShader* vertexShader = SHADERMGR->LoadVertexShader(L"Shaders/Fullscreen/vertex.fx", ShaderType);
	ID3D11InputLayout* inputLayout = SHADERMGR->LoadInputLayout(L"Shaders/Fullscreen/vertex.fx", ShaderType); 

	ID3D11PixelShader* cpyShader = SHADERMGR->LoadPixelShader(L"Shaders/Fullscreen/copy.fx", ShaderType);
	ID3D11PixelShader* cpyShaderR = SHADERMGR->LoadPixelShader(L"Shaders/Fullscreen/copyR.fx", ShaderType);
	ID3D11PixelShader* cpyShaderG = SHADERMGR->LoadPixelShader(L"Shaders/Fullscreen/copyG.fx", ShaderType);
	ID3D11PixelShader* cpyShaderB = SHADERMGR->LoadPixelShader(L"Shaders/Fullscreen/copyB.fx", ShaderType);

	ID3D11PixelShader* HDRShader = SHADERMGR->LoadPixelShader(L"Shaders/Fullscreen/HDR.fx", ShaderType);
	ID3D11PixelShader* avgShader = SHADERMGR->LoadPixelShader(L"Shaders/Fullscreen/average.fx", ShaderType);
	ID3D11PixelShader* addShader = SHADERMGR->LoadPixelShader(L"Shaders/Fullscreen/add.fx", ShaderType);
	ID3D11PixelShader* lumShader = SHADERMGR->LoadPixelShader(L"Shaders/Fullscreen/luminance.fx", ShaderType); //rename the latter shit.
	ID3D11PixelShader* bLuShader = SHADERMGR->LoadPixelShader(L"Shaders/Fullscreen/bloominance.fx", ShaderType); //rename the latter shit.

	ID3D11PixelShader* GBVshader = SHADERMGR->LoadPixelShader(L"Shaders/Fullscreen/gaussianBlurVertical.fx", ShaderType);
	ID3D11PixelShader* GBHshader = SHADERMGR->LoadPixelShader(L"Shaders/Fullscreen/gaussianBlurHorizontal.fx", ShaderType);
	ID3D11PixelShader* tneShader = SHADERMGR->LoadPixelShader(L"Shaders/Fullscreen/tonemap.fx", ShaderType);
	ID3D11PixelShader* mBlShader = SHADERMGR->LoadPixelShader(L"Shaders/Fullscreen/motionBlur.fx", ShaderType);
	ID3D11PixelShader* CLDShader = SHADERMGR->LoadPixelShader(L"Shaders/Fullscreen/cubicLensDistortion.fx", ShaderType);
	ID3D11PixelShader* colorGrading = SHADERMGR->LoadPixelShader(L"Shaders/FullScreen/colorGrading.fx", ShaderType);

	ID3D11PixelShader* fogShader = SHADERMGR->LoadPixelShader(L"Shaders/FullScreen/Fog.fx", ShaderType);


	ID3D11PixelShader* overlay = SHADERMGR->LoadPixelShader(L"Shaders/FullScreen/overlay.fx", ShaderType);
	//Jag är en kommentar också Deferred;

	ID3D11PixelShader* deferredAmbient = SHADERMGR->LoadPixelShader(L"Shaders/Deferred/deferred_ambient.fx", ShaderType);
	ID3D11PixelShader* deferredDirectional = SHADERMGR->LoadPixelShader(L"Shaders/Deferred/deferred_directional.fx", ShaderType);
	//ID3D11PixelShader* deferredPointLight = SHADERMGR->LoadPixelShader(L"Shaders/Deferred/deferred_pointlight.fx", ShaderType);
	ID3D11PixelShader* deferredSpotLight = SHADERMGR->LoadPixelShader(L"Shaders/Deferred/deferred_spotlight.fx", ShaderType);
	ID3D11PixelShader* SSAO = SHADERMGR->LoadPixelShader(L"Shaders/FullScreen/SSAO.fx", ShaderType);

	ID3D11PixelShader* mergeDepth = SHADERMGR->LoadPixelShader(L"Shaders/FullScreen/mergeDepth.fx", ShaderType);

	// LightShafts
	//
	ID3D11VertexShader* lShvertexShader = SHADERMGR->LoadVertexShader(L"Shaders/Fullscreen/lightShafts.fx", ShaderType);
	ID3D11PixelShader* lShShader = SHADERMGR->LoadPixelShader(L"Shaders/Fullscreen/lightShafts.fx", ShaderType);
	ID3D11PixelShader* aaShader = SHADERMGR->LoadPixelShader(L"Shaders/Fullscreen/antiAliasing.fx", ShaderType);

	ID3D11PixelShader* metaSurfaceShader = SHADERMGR->LoadPixelShader(L"Shaders/metaballs/metaSurface.fx", ShaderType);
	ID3D11PixelShader* alphaBlendShader = SHADERMGR->LoadPixelShader(L"Shaders/Fullscreen/alphaBlend.fx", ShaderType);

	myEffects[static_cast<int>(eEffectType::eCopy)]						= new CEffect(vertexShader, cpyShader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eCopyR)]					= new CEffect(vertexShader, cpyShaderR, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eCopyG)]					= new CEffect(vertexShader, cpyShaderG, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eCopyB)]					= new CEffect(vertexShader, cpyShaderB, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	myEffects[static_cast<int>(eEffectType::eHDR)]						= new CEffect(vertexShader, HDRShader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eAverage)]					= new CEffect(vertexShader, avgShader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eAdd)]						= new CEffect(vertexShader, addShader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eLuminance)]				= new CEffect(vertexShader, lumShader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eBloominance)]				= new CEffect(vertexShader, bLuShader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eGaussianBlurVertical)]		= new CEffect(vertexShader, GBVshader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eGaussianBlurHorizontal)]	= new CEffect(vertexShader, GBHshader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eToneMap)]					= new CEffect(vertexShader, tneShader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eMotionBlur)]				= new CEffect(vertexShader, mBlShader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eCubicLensDistortion)]		= new CEffect(vertexShader, CLDShader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eColorGrading)]				= new CEffect(vertexShader, colorGrading, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eFog)]						= new CEffect(vertexShader, fogShader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	//deferred,
	myEffects[static_cast<int>(eEffectType::eDeferredAmbient)]			= new CEffect(vertexShader, deferredAmbient, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eDeferredDirectional)]		= new CEffect(vertexShader, deferredDirectional, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	//myEffects[static_cast<int>(eEffectType::eDeferredPointLight)]		= new CEffect(vertexShader, deferredPointLight, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eDeferredSpotLight)]		= new CEffect(vertexShader, deferredSpotLight, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eSSAO)]						= new CEffect(vertexShader, SSAO, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eAA)]						= new CEffect(vertexShader, aaShader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eMergeDepth)]				= new CEffect(vertexShader, mergeDepth, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

	myEffects[static_cast<int>(eEffectType::eLightShafts)]				= new CEffect(lShvertexShader, lShShader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eMetaSurface)]				= new CEffect(lShvertexShader, metaSurfaceShader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eOverlay)]					= new CEffect(lShvertexShader, overlay, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	myEffects[static_cast<int>(eEffectType::eAlphaBlend)]				= new CEffect(lShvertexShader, alphaBlendShader, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	CreateQuad();
}


CFullScreenHelper::~CFullScreenHelper()
{
	myEffects.DeleteAll();
	myVertexBuffer->Release();
}

void CFullScreenHelper::DoEffect(const eEffectType aEffectType, CRenderPackage * aRenderPackage, CRenderPackage * aSecondRenderPackage)
{
	DoEffect(aEffectType, aRenderPackage->myResource, aSecondRenderPackage != nullptr ? aSecondRenderPackage->myResource : nullptr);
}

void CFullScreenHelper::DoEffect(const eEffectType aEffectType, const CU::Vector4f & aRect, CRenderPackage* aRenderPackage, CRenderPackage* aSecondRenderPackage)
{
	DoEffect(aEffectType, aRect, aRenderPackage->myResource, aSecondRenderPackage != nullptr ? aSecondRenderPackage->myResource : nullptr);
}

void CFullScreenHelper::DoEffect(const eEffectType aEffectType, ID3D11ShaderResourceView * aRenderPackageResource, ID3D11ShaderResourceView * aSecondRenderPackageResource)
{
	myEffects[static_cast<int>(aEffectType)]->Activate();
	UINT stride = sizeof(SVertexData);
	UINT offset = 0;

	DEVICE_CONTEXT->IASetVertexBuffers(0, 1, &myVertexBuffer, &stride, &offset);
	DEVICE_CONTEXT->PSSetShaderResources(1, 1, &aRenderPackageResource); // Send in renderpackage 

	if (aSecondRenderPackageResource != nullptr)
	{
		DEVICE_CONTEXT->PSSetShaderResources(2, 1, &aSecondRenderPackageResource); // Send in renderpackage resource
	}

	DEVICE_CONTEXT->Draw(4, 0);
}

void CFullScreenHelper::DoEffect(const eEffectType aEffectType, const CU::Vector4f & aRect, ID3D11ShaderResourceView * aRenderPackageResource, ID3D11ShaderResourceView * aSecondRenderPackageResource)
{
	if (aRect != CU::Vector4f::Zero)
	{
		CU::Vector2f windowSize;
		windowSize.x = static_cast<float>(ENGINE->GetWindowSize().x);
		windowSize.y = static_cast<float>(ENGINE->GetWindowSize().y);


		D3D11_VIEWPORT activeViewPort;
		unsigned int numPorts = 1u;
 		DEVICE_CONTEXT->RSGetViewports(&numPorts, &activeViewPort);

		if(numPorts != 0u)
		{
			windowSize.x = activeViewPort.Width;
			windowSize.y = activeViewPort.Height;
		}
		CU::Vector4f windowRect;
		windowRect.x = windowSize.x * aRect.x;
		windowRect.y = windowSize.y * aRect.y;
		windowRect.z = windowSize.x * aRect.z;
		windowRect.w = windowSize.y * aRect.w;

		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = windowRect.x;
		viewport.TopLeftY = windowRect.y;
		viewport.Width = windowRect.z - windowRect.x;
		viewport.Height = windowRect.w - windowRect.y;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		DEVICE_CONTEXT->RSSetViewports(1, &viewport);
	}
	DoEffect(aEffectType, aRenderPackageResource, aSecondRenderPackageResource);
}

// Spopa-Sod Edvin Lek här!

void CFullScreenHelper::DoEffect(const eEffectType aEffectType)
{
	myEffects[static_cast<int>(aEffectType)]->Activate();
	UINT stride = sizeof(SVertexData);
	UINT offset = 0;

	DEVICE_CONTEXT->IASetVertexBuffers(0, 1, &myVertexBuffer, &stride, &offset);
	DEVICE_CONTEXT->Draw(4, 0);
}

void CFullScreenHelper::DoEffect(const eEffectType aEffectType, const CU::Vector4f & aRect)
{
	if (aRect != CU::Vector4f::Zero)
	{
		CU::Vector2f windowSize;
		windowSize.x = static_cast<float>(ENGINE->GetWindowSize().x);
		windowSize.y = static_cast<float>(ENGINE->GetWindowSize().y);

		CU::Vector4f windowRect;
		windowRect.x = windowSize.x * aRect.x;
		windowRect.y = windowSize.y * aRect.y;
		windowRect.z = windowSize.x * aRect.z;
		windowRect.w = windowSize.y * aRect.w;

		D3D11_VIEWPORT viewport;
		viewport.TopLeftX = windowRect.x;
		viewport.TopLeftY = windowRect.y;
		viewport.Width = windowRect.z - windowRect.x;
		viewport.Height = windowRect.w - windowRect.y;
		viewport.MinDepth = 0.0f;
		viewport.MaxDepth = 1.0f;
		DEVICE_CONTEXT->RSSetViewports(1, &viewport);
	}
	DoEffect(aEffectType);
}

void CFullScreenHelper::CreateQuad()
{
	CU::GrowingArray<SVertexData> modelVertices;
	modelVertices.Init(8);

	CU::Vector4f topLeft = { -1.0f	, -1.0f , 0.5f, 1.0f };
	CU::Vector4f botLeft = { -1.0f	, 1.0f	, 0.5f, 1.0f };
	CU::Vector4f topRight = { 1.0f	, -1.0f , 0.5f, 1.0f };
	CU::Vector4f botRight = { 1.0f	, 1.0f	, 0.5f, 1.0f };

	CU::Vector2f UVtopLeft	= { 0.0f	, 1.0f };
	CU::Vector2f UVbotLeft	= { 0.0f	, 0.0f	};
	CU::Vector2f UVtopRight = { 1.0f	, 1.0f };
	CU::Vector2f UVbotRight = { 1.0f	, 0.0f	};
	
	modelVertices.Add({ topLeft	,	UVtopLeft });
	modelVertices.Add({ botLeft	,	UVbotLeft });
	modelVertices.Add({ topRight,	UVtopRight });
	modelVertices.Add({ botRight,	UVbotRight });

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = modelVertices.Size() * sizeof(SVertexData);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = &modelVertices[0];
	HRESULT result;
	result = DEVICE->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
	CHECK_RESULT(result, "Failed to create vertexbuffer.");
}

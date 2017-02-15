#include "stdafx.h"
#include "StreakEmitter.h"
#include "VertexStructs.h"
#include "Effect.h"
#include "Engine.h"
#include "ShaderManager.h"
#include <d3d11.h>
#include "DXFramework.h"
#include "ConstBufferTemplate.h"
#include <CommonUtilities.h>
#include "../CommonUtilities/CUTime.h"
#include <Camera.h>
#include "TextureManager.h"


namespace
{
	float fLerp(float aStart, float aEnd, float aTime)
	{
		return (aEnd + aTime*(aStart - aEnd));
	}
}

CStreakEmitter::CStreakEmitter(const SStreakEmitterData& aEmitterData)
{
	myTexture = nullptr;
	myEffect = nullptr;

	myFramework = nullptr;
	myVertexBuffer = nullptr;

	myVertexCBuffer = nullptr;



	myEmitDelta = 0.f;
	myEmitTimer = 0.f;
	myIsActivated = true;

	myEmitterData = aEmitterData;
}

CStreakEmitter::~CStreakEmitter()
{
	SAFE_DELETE(myEffect);

	if (myTexture != nullptr)
	{
		TEXTUREMGR.DestroyTexture(myTexture);
	}

	SAFE_RELEASE(myVertexBuffer);
	SAFE_RELEASE(myGeometryCBuffer);
	SAFE_RELEASE(myVertexCBuffer);

	myIsActivated = false;
}

bool CStreakEmitter::InitBuffers()
{
	//VertexBuffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(SStreak) * ourMaxNrOfParticles;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;

	HRESULT result;
	result = DEVICE->CreateBuffer(&vertexBufferDesc, nullptr, &myVertexBuffer);
	CHECK_RESULT(result, "Failed to create vertex buffer.");

	//Cbuffers
	SToWorldSpace worldBufferData;
	myVertexCBuffer = BSR::CreateCBuffer<SToWorldSpace>(&worldBufferData);

	SCameraPosition cameraData;
	myGeometryCBuffer = BSR::CreateCBuffer<SCameraPosition>(&cameraData);

	return true;
}

void CStreakEmitter::Init()
{
	myParticles.Init(100);
	myParticleLogic.Init(100);

	myEmitDelta = 1.f / myEmitterData.EmissionRate; // rate of emission
	myEmitTimer = 0.f;

	myTexture = &TEXTUREMGR.LoadTexture(myEmitterData.TexturePath);

	unsigned int ShaderType = 0;
	ShaderType |= EModelBluePrint_Position;
	ShaderType |= EModelBluePrint_Size;
	ShaderType |= EModelBluePrint_Alpha;

	ID3D11VertexShader* vertexShader = SHADERMGR->LoadVertexShader(L"Shaders/streak_shader.fx", ShaderType);
	ID3D11GeometryShader* geometryShader = SHADERMGR->LoadGeometryShader(L"Shaders/streak_shader.fx", ShaderType);
	ID3D11InputLayout* inputLayout = SHADERMGR->LoadInputLayout(L"Shaders/streak_shader.fx", ShaderType); //TODO: kompilera shitN i förväg, så vi slipper skicka in filePath här.


																										  //if texture is defined
	ShaderType = 0;
	ShaderType |= EModelBluePrint_Position;
	ShaderType |= EModelBluePrint_UV;

	ID3D11PixelShader* pixelShader = SHADERMGR->LoadPixelShader(L"Shaders/streak_shader.fx", ShaderType);

	CEffect* effect = new CEffect(vertexShader, pixelShader, geometryShader, inputLayout, D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_LINESTRIP_ADJ);
	myEffect = effect;

	myFramework = FRAMEWORK;
	InitBuffers();
}

void CStreakEmitter::Update(const CU::Time& aDeltaTime, const CU::Matrix44f& aToWorld)
{
	float deltaTime = aDeltaTime.GetSeconds();
	myEmitTimer += deltaTime;

	if (myIsActivated == true)
	{
		if (myEmitTimer >= myEmitDelta)
		{
			myEmitTimer -= myEmitDelta;
			EmitParticle(aToWorld);
		}
	}


	if (myParticles.Size() != myParticleLogic.Size())
	{
		assert(false && "Particlelist and logiclist missalligned.");
	}

	for (unsigned int i = 0; i < myParticles.Size(); ++i)
	{
		myParticleLogic[i].lifeLeft -= deltaTime;
		if (myParticleLogic[i].lifeLeft <= 0.0f)
		{
			myParticles.RemoveAtIndex(i);
			myParticleLogic.RemoveAtIndex(i);
			--i;
			continue;
		}

		myParticles[i].size = fLerp(myEmitterData.StartSize, myEmitterData.EndSize, myParticleLogic[i].lifeLeft / myParticleLogic[i].lifeTime);
		myParticles[i].alpha = fLerp(myEmitterData.StartAlpha, myEmitterData.EndAlpha, myParticleLogic[i].lifeLeft / myParticleLogic[i].lifeTime);
	}

}

void CStreakEmitter::EmitParticle(const CU::Matrix44f& aToWorld)
{
	SStreak particle;
	particle.alpha = myEmitterData.StartAlpha;
	particle.pos = aToWorld.GetPosition();
	particle.pos.w = 1.0f;
	particle.size = myEmitterData.StartSize;

	SStreakLogic logic;
	logic.lifeTime = myEmitterData.ParticleLifetime;
	logic.lifeLeft = logic.lifeTime;

	myParticles.Add(particle);
	myParticleLogic.Add(logic);
}

void CStreakEmitter::UpdateCBuffers(const CU::Matrix44f & /*aToWorldSpace*/) //TODO: remove this world space parameter
{
	////WorldSpace
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	//ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	//SToWorldSpace updatedWorld;
	//updatedWorld.myWorldSpace = aToWorldSpace;
	//DEVICE_CONTEXT->Map(myVertexCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	//memcpy(mappedSubresource.pData, &updatedWorld, sizeof(SToWorldSpace));
	//DEVICE_CONTEXT->Unmap(myVertexCBuffer, 0);
	//DEVICE_CONTEXT->VSSetConstantBuffers(1, 1, &myVertexCBuffer);

	//CameraPos
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	SCameraPosition updatedCamera;
	//updatedCamera.position = CAMERA->GetPosition(); //TODO: ADD REAL CAMERA
	DEVICE_CONTEXT->Map(myGeometryCBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, &updatedCamera, sizeof(SCameraPosition));
	DEVICE_CONTEXT->Unmap(myGeometryCBuffer, 0);
	DEVICE_CONTEXT->GSSetConstantBuffers(1, 1, &myGeometryCBuffer);

	//VertexBuffer
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	DEVICE_CONTEXT->Map(myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);

	unsigned int bufferSize;
	bufferSize = sizeof(SStreak);

	if (myParticles.Size() > ourMaxNrOfParticles)
		bufferSize *= ourMaxNrOfParticles;
	else
		bufferSize *= myParticles.Size();


	if (myParticles.Size() > 0)
		memcpy(mappedSubresource.pData, &myParticles[0], bufferSize);

	DEVICE_CONTEXT->Unmap(myVertexBuffer, 0);
}

void CStreakEmitter::Render(const CU::Matrix44f& aToWorld)
{
	if (myParticles.Size() > 0)
	{
		myEffect->Activate();
		UpdateCBuffers(aToWorld);

		DEVICE_CONTEXT->PSSetShaderResources(1, 1, myTexture->GetShaderResourceViewPointer());

		UINT stride = sizeof(SStreak);
		UINT offset = 0;
		DEVICE_CONTEXT->IASetVertexBuffers(0, 1, &myVertexBuffer, &stride, &offset);
		DEVICE_CONTEXT->IASetIndexBuffer(NULL, DXGI_FORMAT_R32_UINT, NULL);

		DEVICE_CONTEXT->Draw(myParticles.Size(), 0);
	}
}

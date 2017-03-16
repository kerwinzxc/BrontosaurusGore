#include "stdafx.h"
#include "ParticleEmitter.h"
#include "EmitterData.h"

#include "ShaderManager.h"
#include "Effect.h"
#include "VertexStructs.h"

#include "DXFramework.h"
#include "ConstBufferTemplate.h"
#include "TextureManager.h"
#include <mutex>

CParticleEmitter::CParticleEmitter()
{
	myMaxNrOfParticles = 0;
	myTexture = nullptr;
	
	for(int i = 0; i < myRenderEffects.Size(); ++i)
	{
		myRenderEffects[i] = nullptr;
	}

	myFramework = nullptr;
	myVertexBuffer = nullptr;
	myModelBuffer = nullptr;
}

CParticleEmitter::CParticleEmitter(const CParticleEmitter& aParticleEmitter)
{
	*this = aParticleEmitter;
}

CParticleEmitter::~CParticleEmitter()
{
	Destroy();
}

void CParticleEmitter::Init(const SEmitterData& aEmitterData)
{
	myTexture = &TEXTUREMGR.LoadTexture(aEmitterData.TexturePath.c_str());
	myMaxNrOfParticles = static_cast<unsigned short>(aEmitterData.NumOfParticles);


	unsigned int ShaderType = 0;
	ShaderType |= EModelBluePrint_Position;
	ShaderType |= EModelBluePrint_Size;
	ShaderType |= EModelBluePrint_Color;
	
	ID3D11VertexShader* vertexShader = SHADERMGR->LoadVertexShader(L"Shaders/metaballs/metaballsVertex.fx", ShaderType);
	ID3D11PixelShader* pixelShader = SHADERMGR->LoadPixelShader(L"Shaders/metaballs/metaballsSpherePixel.fx", ShaderType);
	ID3D11GeometryShader* geometryShader = SHADERMGR->LoadGeometryShader(L"Shaders/metaballs/metaballsGeometry.fx", ShaderType);
	ID3D11InputLayout* inputLayout = SHADERMGR->LoadInputLayout(L"Shaders/metaballs/metaballsVertex.fx", ShaderType);
	
	myRenderEffects[static_cast<int>(RenderMode::eMetaBall)] = new CEffect(vertexShader, pixelShader, geometryShader, inputLayout, D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	vertexShader = SHADERMGR->LoadVertexShader(L"Shaders/nurbs/nurbsVertex.fx", ShaderType);
	pixelShader = SHADERMGR->LoadPixelShader(L"Shaders/nurbs/nurbsSpherePixel.fx", ShaderType);
	geometryShader = SHADERMGR->LoadGeometryShader(L"Shaders/nurbs/nurbsGeometry.fx", ShaderType);
	inputLayout = SHADERMGR->LoadInputLayout(L"Shaders/nurbs/nurbsVertex.fx", ShaderType);

	myRenderEffects[static_cast<int>(RenderMode::eNURBSSphere)] = 
		new CEffect(vertexShader, pixelShader, geometryShader, inputLayout, D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	myFramework = FRAMEWORK;
	InitBuffers();
}

void CParticleEmitter::Render(const CU::Matrix44f & aToWorldSpace, const CU::GrowingArray<SParticle, unsigned int, false>& aParticleList, RenderMode aRenderMode)
{
	if (!myRenderEffects[static_cast<int>(aRenderMode)]) return;

	myRenderEffects[static_cast<int>(aRenderMode)]->Activate();
	UpdateCBuffers(aToWorldSpace);
	ResizeVertexBuffer(aParticleList);

	DEVICE_CONTEXT->PSSetShaderResources(1, 1, myTexture->GetShaderResourceViewPointer());

	UINT stride = sizeof(SParticle);
	UINT offset = 0;
	ID3D11Buffer* vertexBuffer = GetVertexBuffer();
	myFramework->GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	myFramework->GetDeviceContext()->Draw(aParticleList.Size(), 0);
}

void CParticleEmitter::Destroy()
{
	if (myTexture)
	{
		CTextureManager& textureManager = TEXTUREMGR;
		textureManager.DestroyTexture(myTexture);
	}

	SAFE_RELEASE(myModelBuffer);
	SAFE_RELEASE(myVertexBuffer);
	for(int i = 0; i < myRenderEffects.Size(); ++i)
	{
		SAFE_DELETE(myRenderEffects[i]);
	}

}

bool CParticleEmitter::HasEffects() const
{
	for(int i = 0; i < myRenderEffects.Size(); ++i)
	{
		if(myRenderEffects[i] != nullptr)
		{
			return true;
		}
	}
	return false;
}

CParticleEmitter& CParticleEmitter::operator=(const CParticleEmitter& aParticleEmitter)
{
	if (!aParticleEmitter.HasEffects())
	{
		Destroy();
		return *this;
	}

	myMaxNrOfParticles = aParticleEmitter.myMaxNrOfParticles;
	myTexture = aParticleEmitter.myTexture;
	SAFE_ADD_REF(myTexture);

	for(int i = 0; i < myRenderEffects.Size(); ++i)
	{
		if(aParticleEmitter.myRenderEffects[i] != nullptr)
		{
			myRenderEffects[i] = new CEffect(*aParticleEmitter.myRenderEffects[i]);
		}
	}

	myFramework = aParticleEmitter.myFramework;

	myVertexBuffer = aParticleEmitter.myVertexBuffer;
	SAFE_ADD_REF(myVertexBuffer);
	myModelBuffer = aParticleEmitter.myModelBuffer;
	SAFE_ADD_REF(myModelBuffer);

	return *this;
}

CParticleEmitter& CParticleEmitter::operator=(CParticleEmitter&& aParticleEmitter)
{
	myMaxNrOfParticles = aParticleEmitter.myMaxNrOfParticles;
	myTexture = aParticleEmitter.myTexture; for (int i = 0; i < myRenderEffects.Size(); ++i)
	{
		if (aParticleEmitter.myRenderEffects[i] != nullptr)
		{
			myRenderEffects[i] = aParticleEmitter.myRenderEffects[i];
		}
	}
	myFramework = aParticleEmitter.myFramework;
	myVertexBuffer = aParticleEmitter.myVertexBuffer;
	myModelBuffer = aParticleEmitter.myModelBuffer;

	aParticleEmitter.myMaxNrOfParticles = 0;
	aParticleEmitter.myTexture = nullptr;
	for(int i = 0; i < aParticleEmitter.myRenderEffects.Size();++i)
	{
		aParticleEmitter.myRenderEffects[i] = nullptr;
	}
	aParticleEmitter.myFramework = nullptr;
	aParticleEmitter.myVertexBuffer = nullptr;
	aParticleEmitter.myModelBuffer = nullptr;

	return *this;
}

ID3D11Buffer* CParticleEmitter::GetVertexBuffer()
{
	std::lock_guard<std::mutex> lock(myUpdateVBufferMutex);
	return myVertexBuffer;
}

void CParticleEmitter::ResizeVertexBuffer(const CU::GrowingArray<SParticle, unsigned int, false>& aParticleList)
{
	if (aParticleList.Empty()) return;

	unsigned int bufferSize = sizeof(SParticle);

	if (aParticleList.Size() > myMaxNrOfParticles)
	{
		bufferSize *= myMaxNrOfParticles;
	}
	else
	{
		bufferSize *= aParticleList.Size();
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	
	DEVICE_CONTEXT->Map(myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	std::lock_guard<std::mutex> lock(myUpdateVBufferMutex);
	memcpy(resource.pData, &aParticleList[0], bufferSize);

	DEVICE_CONTEXT->Unmap(myVertexBuffer, 0);
}

void CParticleEmitter::UpdateCBuffers(const CU::Matrix44f & aToWorldSpace)
{
	//WorldSpace
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	SToWorldSpace updatedWorld;
	updatedWorld.myWorldSpace = aToWorldSpace;

	DEVICE_CONTEXT->Map(myModelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, &updatedWorld, sizeof(SToWorldSpace));
	DEVICE_CONTEXT->Unmap(myModelBuffer, 0);
	DEVICE_CONTEXT->VSSetConstantBuffers(1, 1, &myModelBuffer);
}

bool CParticleEmitter::InitBuffers()
{
	//VertexBuffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(SParticle) * myMaxNrOfParticles;
	vertexBufferDesc.BindFlags = D3D11_USAGE_DYNAMIC | D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;

	HRESULT result;
	result = DEVICE->CreateBuffer(&vertexBufferDesc, nullptr, &myVertexBuffer);
	CHECK_RESULT(result, "Failed to create vertex buffer.");

	SAFE_RELEASE(myModelBuffer);
	//Cbuffers
	SToWorldSpace worldBufferData;
	myModelBuffer = BSR::CreateCBuffer<SToWorldSpace>(&worldBufferData);

	return true;
}

#include "stdafx.h"
#include "FireEmitter.h"

#include "Effect.h"
#include "Texture.h"
#include "ShaderManager.h"
#include "TextureManager.h"
#include "EModelBluePrint.h"

#include <d3d11.h>

#define RING_BRANDBILEN() assert(!"Fail in fire emitter")
#define self (*this)

struct SNoiseBufferStruct
{
	float frameTime;
	float scrollSpeeds[3];
	float scales[3];
	float garbage;
};

struct SDistortionBufferStruct
{
	CU::Vector2f distortions[3];
	float distortionScale;
	float distortionBias;
};

CFireEmitter::CFireEmitter()
	: myEffect(nullptr)
	, myVertexBuffer(nullptr)
	, myIndexBuffer(nullptr)
	, myMatrixBuffer(nullptr)
	, myNoiseBuffer(nullptr)
	, myDistortionBuffer(nullptr)
	, myFireTexture(nullptr)
	, myNoiseTexture(nullptr)
	, myAlphaTexture(nullptr)
	, myRefCount(0)
{
	memset(&myFireEmitterData, 0, sizeof(myFireEmitterData));
}

CFireEmitter::CFireEmitter(const CFireEmitter& aCopy)
{
	self = aCopy;
}

CFireEmitter::CFireEmitter(CFireEmitter&& aTemporary)
{
	self = std::move(aTemporary);
}

CFireEmitter::~CFireEmitter()
{
	Destroy();
}

CFireEmitter& CFireEmitter::operator=(const CFireEmitter& aCopy)
{
	Destroy();

	myEffect = new CEffect(*aCopy.myEffect);
	
	myVertexBuffer = aCopy.myVertexBuffer;
	SAFE_ADD_REF(myVertexBuffer);

	myIndexBuffer = aCopy.myIndexBuffer;
	SAFE_ADD_REF(myIndexBuffer);

	myMatrixBuffer = aCopy.myMatrixBuffer;
	SAFE_ADD_REF(myMatrixBuffer);

	myNoiseBuffer = aCopy.myNoiseBuffer;
	SAFE_ADD_REF(myNoiseBuffer);

	myDistortionBuffer = aCopy.myDistortionBuffer;
	SAFE_ADD_REF(myDistortionBuffer);

	myFireTexture = aCopy.myFireTexture;
	myFireTexture->AddRef();

	myNoiseTexture = aCopy.myNoiseTexture;
	myNoiseTexture->AddRef();

	myAlphaTexture = aCopy.myAlphaTexture;
	myAlphaTexture->AddRef();

	return self;
}

CFireEmitter& CFireEmitter::operator=(CFireEmitter&& aTemporary)
{
	Destroy();

	myEffect = aTemporary.myEffect;
	aTemporary.myEffect = nullptr;

	myVertexBuffer = aTemporary.myVertexBuffer;
	aTemporary.myVertexBuffer = nullptr;

	myIndexBuffer = aTemporary.myIndexBuffer;
	aTemporary.myIndexBuffer = nullptr;

	myMatrixBuffer = aTemporary.myMatrixBuffer;
	aTemporary.myMatrixBuffer = nullptr;

	myNoiseBuffer = aTemporary.myNoiseBuffer;
	aTemporary.myNoiseBuffer = nullptr;

	myDistortionBuffer = aTemporary.myDistortionBuffer;
	aTemporary.myDistortionBuffer = nullptr;

	myFireTexture = aTemporary.myFireTexture;
	aTemporary.myFireTexture = nullptr;

	myNoiseTexture = aTemporary.myNoiseTexture;
	aTemporary.myNoiseTexture = nullptr;

	myAlphaTexture = aTemporary.myAlphaTexture;
	aTemporary.myAlphaTexture = nullptr;

	return self;
}

void CFireEmitter::Init(const SFireEmitterData& aData)
{
	if (InitInputBuffers() == false)
	{
		return;
	}

	if (InitConstantBuffers() == false)
	{
		return;
	}

	if (InitTextures(aData) == false)
	{
		return;
	}

	memcpy(&myFireEmitterData, &aData, sizeof(SFireEmitterData));
}

void CFireEmitter::Destroy()
{
	SAFE_DELETE(myEffect);

	SAFE_RELEASE(myVertexBuffer);
	SAFE_RELEASE(myIndexBuffer);

	SAFE_RELEASE(myMatrixBuffer);
	SAFE_RELEASE(myNoiseBuffer);
	SAFE_RELEASE(myDistortionBuffer);

	CTextureManager& textureMan = TEXTUREMGR;
	textureMan.DestroyTexture(myFireTexture);
	textureMan.DestroyTexture(myNoiseTexture);
	textureMan.DestroyTexture(myAlphaTexture);
}

void CFireEmitter::Render(const CU::Time aFrameTime, const CU::Matrix44f& aToWorldMatrix)
{
	UpdateBuffers(aFrameTime, aToWorldMatrix);
	SetTextures();
	myEffect->Activate();

	ID3D11DeviceContext* deviceContext = DEVICE_CONTEXT;
	if (deviceContext == nullptr) return;

	UINT stride = ourVertexSize;
	UINT offset = 0;

	deviceContext->IASetVertexBuffers(0, 1, &myVertexBuffer, &stride, &offset);
	deviceContext->IASetIndexBuffer(myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	deviceContext->DrawIndexed(ourIndexCount, 0, 0);
}

bool CFireEmitter::InitInputBuffers()
{
	ID3D11Device* device = DEVICE;
	if (device == nullptr) return false;


	// VERTICES
	struct SFireVertex
	{
		CU::Vector4f position;
		CU::Vector2f uv;
	} vertices[4];

	vertices[0].position.Set(-1.f, -1.f, 0.f, 1.f);
	vertices[0].uv.Set(0.f, 1.f);
	vertices[1].position.Set( 1.f, -1.f, 0.f, 1.f);
	vertices[1].uv.Set(1.f, 1.f);
	vertices[2].position.Set(-1.f,  1.f, 0.f, 1.f);
	vertices[2].uv.Set(0.f, 0.f);
	vertices[3].position.Set( 1.f,  1.f, 0.f, 1.f);
	vertices[3].uv.Set(1.f, 0.f);

	// INDICES
	unsigned int indices[6];
	indices[0] = 2;
	indices[1] = 3;
	indices[2] = 0;
	indices[3] = 0;
	indices[4] = 3;
	indices[5] = 1;


	// MESSING AROUND BC EVERYGHING IS BIG AS FUCK IN OUR GAME
	for (auto& vertex : vertices)
	{
		vertex.position.x *= 160.f;
		vertex.position.y *= 200.f;
	}



	// VERTEX BUFFER
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(vertices);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = vertices;
	HRESULT result;
	result = device->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
	if (FAILED(result))
	{
		RING_BRANDBILEN();
		return false;
	}


	// INDEX BUFFER
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(indices);
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = indices;
	result = DEVICE->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
	if (FAILED(result))
	{
		SAFE_RELEASE(myVertexBuffer);
		RING_BRANDBILEN();
		return false;
	}

	return true;
}

bool CFireEmitter::InitConstantBuffers()
{
	CShaderManager* shaderManager = SHADERMGR;
	if (shaderManager == nullptr) return false;

	ID3D11Device* device = DEVICE;
	if (device == nullptr) return false;

	ID3D11VertexShader* vertexShader = shaderManager->LoadVertexShader(L"Shaders/fire_shader.fx", EModelBluePrint_Fire);
	ID3D11PixelShader* pixelShader = shaderManager->LoadPixelShader(L"Shaders/fire_shader.fx", EModelBluePrint_Fire);
	ID3D11InputLayout* layout = shaderManager->LoadInputLayout(L"Shaders/fire_shader.fx", EModelBluePrint_Fire);
	myEffect = new CEffect(vertexShader, pixelShader, nullptr, layout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	D3D11_BUFFER_DESC toWorldBufferDesc = {};
	toWorldBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	toWorldBufferDesc.ByteWidth = sizeof(CU::Matrix44f);
	toWorldBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	toWorldBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	toWorldBufferDesc.MiscFlags = 0;
	toWorldBufferDesc.StructureByteStride = 0;

	HRESULT result = device->CreateBuffer(&toWorldBufferDesc, nullptr, &myMatrixBuffer);
	if (FAILED(result))
	{
		DL_PRINT_WARNING("Failed to create to world matrix buffer for fire shader");
		SAFE_DELETE(myEffect);
		RING_BRANDBILEN();
		return false;
	}

	D3D11_BUFFER_DESC noiseBufferDesc = {};
	noiseBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	noiseBufferDesc.ByteWidth = sizeof(SNoiseBufferStruct);
	noiseBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	noiseBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	noiseBufferDesc.MiscFlags = 0;
	noiseBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&noiseBufferDesc, nullptr, &myNoiseBuffer);
	if (FAILED(result))
	{
		//1. print to log
		DL_PRINT_WARNING("Failed to create noise buffer for fire shader");
		SAFE_DELETE(myEffect);
		SAFE_RELEASE(myMatrixBuffer);
		RING_BRANDBILEN();
		return false;
	}

	D3D11_BUFFER_DESC distortionBufferDesc = {};
	distortionBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	distortionBufferDesc.ByteWidth = sizeof(SDistortionBufferStruct);
	distortionBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	distortionBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	distortionBufferDesc.MiscFlags = 0;
	distortionBufferDesc.StructureByteStride = 0;

	result = device->CreateBuffer(&distortionBufferDesc, nullptr, &myDistortionBuffer);
	if (FAILED(result))
	{
		//1. print to log
		DL_PRINT_WARNING("Failed to create distortion buffer for fire shader");
		SAFE_DELETE(myEffect);
		SAFE_RELEASE(myMatrixBuffer);
		SAFE_RELEASE(myNoiseBuffer);
		RING_BRANDBILEN();
		return false;
	}

	return true;
}

bool CFireEmitter::InitTextures(const SFireEmitterData& aData)
{
	CTextureManager& textureMan = TEXTUREMGR;

	myFireTexture = &textureMan.LoadTexture(aData.myColorTexturePath.c_str());
	myNoiseTexture = &textureMan.LoadTexture(aData.myNoiseTexturePath.c_str());
	myAlphaTexture = &textureMan.LoadTexture(aData.myAlphaTexturePath.c_str());
	
	return true;
}

void CFireEmitter::UpdateBuffers(const CU::Time aFrameTime, const CU::Matrix44f& aToWorldMatrix)
{
	ID3D11DeviceContext* deviceContext = DEVICE_CONTEXT;
	if (deviceContext == nullptr) return;

	HRESULT result = S_OK;


	//toworld matrix buffer
	D3D11_MAPPED_SUBRESOURCE mappedMatrixBuffer = {};
	result = deviceContext->Map(myMatrixBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedMatrixBuffer);
	if (FAILED(result))
	{
		RING_BRANDBILEN();
		return;
	}
	memcpy(mappedMatrixBuffer.pData, &aToWorldMatrix, sizeof(aToWorldMatrix));
	deviceContext->Unmap(myMatrixBuffer, 0);


	//noise buffer
	SNoiseBufferStruct updatedNoiseBuffer = {};
	updatedNoiseBuffer.frameTime = aFrameTime.GetSeconds() * 0.5f;
	memcpy(updatedNoiseBuffer.scrollSpeeds, myFireEmitterData.myScrollSpeeds, sizeof(float) * 3u);
	memcpy(updatedNoiseBuffer.scales, myFireEmitterData.myScales, sizeof(float) * 3u);

	D3D11_MAPPED_SUBRESOURCE mappedNoiseResource = {};
	result = deviceContext->Map(myNoiseBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedNoiseResource);
	if (FAILED(result))
	{
		RING_BRANDBILEN();
		return;
	}

	memcpy(mappedNoiseResource.pData, &updatedNoiseBuffer, sizeof(updatedNoiseBuffer));
	deviceContext->Unmap(myNoiseBuffer, 0);

	ID3D11Buffer* const vertexCBuffers[2] = { myMatrixBuffer, myNoiseBuffer };
	deviceContext->VSSetConstantBuffers(1, 2, vertexCBuffers);

	//distortion buffer
	SDistortionBufferStruct updatedDistortion = {};
	memcpy(updatedDistortion.distortions, myFireEmitterData.myDistortions, sizeof(float) * 3u);
	updatedDistortion.distortionScale = myFireEmitterData.myDistortionScale;
	updatedDistortion.distortionBias = myFireEmitterData.myDistortionBias;

	D3D11_MAPPED_SUBRESOURCE mappedDistortionResource = {};
	result = deviceContext->Map(myDistortionBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedDistortionResource);
	if (FAILED(result))
	{
		RING_BRANDBILEN();
		return;
	}
	memcpy(mappedDistortionResource.pData, &updatedDistortion, sizeof(updatedDistortion));
	deviceContext->Unmap(myDistortionBuffer, 0);

	deviceContext->PSSetConstantBuffers(1, 1, &myDistortionBuffer);

	//return true;
}

void CFireEmitter::SetTextures()
{
	ID3D11DeviceContext* deviceContext = DEVICE_CONTEXT;
	if (deviceContext == nullptr) return;

	ID3D11ShaderResourceView* const textures[3] = { myFireTexture->GetShaderResourceView(), myNoiseTexture->GetShaderResourceView(), myAlphaTexture->GetShaderResourceView() };
	deviceContext->PSSetShaderResources(1, 3, textures);
}

#undef self
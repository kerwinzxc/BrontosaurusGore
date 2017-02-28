#include "stdafx.h"
#include "Skybox.h"
#include "DXFramework.h"
#include "Effect.h"
#include "BufferStructs.h"
#include "Engine.h"
#include "TextureManager.h"
#include "ConstBufferTemplate.h"
#include <Camera.h>
#include <matrix44.h>
#include "DDSTextureLoader.h"
#include "ShaderManager.h"

CSkybox::CSkybox()
	: myEffect(nullptr)
	, myVSBuffer(nullptr)
	, myPSBuffer(nullptr)
	, myVertexBuffer(nullptr)
	, myIndexBuffer(nullptr)
	, mySkyboxTexture(nullptr)
	, mySRV(nullptr)
	, myRefCount(1)
{
}

CSkybox::~CSkybox()
{
	SAFE_RELEASE(myIndexBuffer);
	SAFE_RELEASE(myVertexBuffer);
	SAFE_RELEASE(myVSBuffer);
	SAFE_RELEASE(myPSBuffer);
	SAFE_DELETE(myEffect);

	TEXTUREMGR.DestroyTexture(mySkyboxTexture);
}

void CSkybox::Init(const char* aPath)
{
	unsigned int shaderType = 1; // position
	ID3D11VertexShader* vertexShader = SHADERMGR->LoadVertexShader(L"Shaders/skybox_shader.fx", shaderType);
	ID3D11PixelShader* pixelShader = SHADERMGR->LoadPixelShader(L"Shaders/skybox_shader.fx", shaderType);
	ID3D11GeometryShader* geometryShader = nullptr;//SHADERMGR->LoadGeometryShader(L"Shaders/skybox_shader.fx", shaderType);
	ID3D11InputLayout* inputLayout = SHADERMGR->LoadInputLayout(L"Shaders/skybox_shader.fx", shaderType);
	D3D_PRIMITIVE_TOPOLOGY topology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	myEffect = new CEffect(vertexShader, pixelShader, geometryShader, inputLayout, topology);

	mySkyboxTexture = &TEXTUREMGR.LoadTexture(aPath);
	mySRV = nullptr;

	CreateVertexIndexBuffer();
	CU::Matrix44f VSbuffer;
	myVSBuffer = BSR::CreateCBuffer<CU::Matrix44f>(&VSbuffer);
	CU::Vector4f PSBuffer;
	myPSBuffer = BSR::CreateCBuffer<CU::Vector4f>(&PSBuffer);
}

void CSkybox::Init(ID3D11ShaderResourceView* aSRV)
{
	unsigned int shaderType = 1; // position
	ID3D11VertexShader* vertexShader = SHADERMGR->LoadVertexShader(L"Shaders/skybox_shader.fx", shaderType);
	ID3D11PixelShader* pixelShader = SHADERMGR->LoadPixelShader(L"Shaders/skybox_shader.fx", shaderType);
	ID3D11GeometryShader* geometryShader = nullptr;//SHADERMGR->LoadGeometryShader(L"Shaders/skybox_shader.fx", shaderType);
	ID3D11InputLayout* inputLayout = SHADERMGR->LoadInputLayout(L"Shaders/skybox_shader.fx", shaderType);
	D3D_PRIMITIVE_TOPOLOGY topology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	myEffect = new CEffect(vertexShader, pixelShader, geometryShader, inputLayout, topology);

	mySkyboxTexture = nullptr;
	mySRV = aSRV;

	CreateVertexIndexBuffer();
	CU::Matrix44f VSbuffer;
	myVSBuffer = BSR::CreateCBuffer<CU::Matrix44f>(&VSbuffer);
	CU::Vector4f PSBuffer;
	myPSBuffer = BSR::CreateCBuffer<CU::Vector4f>(&PSBuffer);
}

void CSkybox::Render(const CU::Camera& aCamera)
{
	if (!myEffect) return;

	myEffect->Activate();
	UpdateCbuffer(aCamera);

	DEVICE_CONTEXT->PSSetShaderResources(0, 1, mySRV == nullptr ? mySkyboxTexture->GetShaderResourceViewPointer() : &mySRV);
	DEVICE_CONTEXT->PSSetShaderResources(1, 1, mySRV == nullptr ? mySkyboxTexture->GetShaderResourceViewPointer() : &mySRV);


	UINT stride = sizeof(SVertexDataCube);
	UINT offset = 0;
	DEVICE_CONTEXT->IASetVertexBuffers(0, 1, &myVertexBuffer, &stride, &offset);
	DEVICE_CONTEXT->IASetIndexBuffer(myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DEVICE_CONTEXT->DrawIndexed(36, 0, 0);
}

void CSkybox::CreateVertexIndexBuffer()
{
	CU::GrowingArray<SVertexDataCube> modelVertices;// = aModel->GetVertices();
	CU::GrowingArray<unsigned int> modelIndices;// = aModel->GetIndices();

	modelVertices.Init(8);
	modelIndices.Init(36);
	    
	const float size = 100.0f;

	modelVertices.Add(SVertexDataCube());
	modelVertices[0].position.x = -size;
	modelVertices[0].position.y = size;
	modelVertices[0].position.z = -size;
	modelVertices[0].position.w = 1.0f;


	modelVertices.Add(SVertexDataCube());
	modelVertices[1].position.x = -size;
	modelVertices[1].position.y = size;
	modelVertices[1].position.z = size;
	modelVertices[1].position.w = 1.0f;

	modelVertices.Add(SVertexDataCube());
	modelVertices[2].position.x = size;
	modelVertices[2].position.y = size;
	modelVertices[2].position.z = size;
	modelVertices[2].position.w = 1.0f;

	modelVertices.Add(SVertexDataCube());
	modelVertices[3].position.x = size;
	modelVertices[3].position.y = size;
	modelVertices[3].position.z = -size;
	modelVertices[3].position.w = 1.0f;


	// BOT
	modelVertices.Add(SVertexDataCube());
	modelVertices[4].position.x = -size;
	modelVertices[4].position.y = -size;
	modelVertices[4].position.z = -size;
	modelVertices[4].position.w = 1.0f;

	modelVertices.Add(SVertexDataCube());
	modelVertices[5].position.x = -size;
	modelVertices[5].position.y = -size;
	modelVertices[5].position.z = size;
	modelVertices[5].position.w = 1.0f;

	modelVertices.Add(SVertexDataCube());
	modelVertices[6].position.x = size;
	modelVertices[6].position.y = -size;
	modelVertices[6].position.z = size;
	modelVertices[6].position.w = 1.0f;

	modelVertices.Add(SVertexDataCube());
	modelVertices[7].position.x = size;
	modelVertices[7].position.y = -size;
	modelVertices[7].position.z = -size;
	modelVertices[7].position.w = 1.0f;

	modelIndices.Add(0);
	modelIndices.Add(1);
	modelIndices.Add(2);
	modelIndices.Add(0);
	modelIndices.Add(2);
	modelIndices.Add(3);

	modelIndices.Add(4);
	modelIndices.Add(6);
	modelIndices.Add(5);
	modelIndices.Add(4);
	modelIndices.Add(7);
	modelIndices.Add(6);

	modelIndices.Add(4);
	modelIndices.Add(5);
	modelIndices.Add(1);
	modelIndices.Add(4);
	modelIndices.Add(1);
	modelIndices.Add(0);

	modelIndices.Add(6);
	modelIndices.Add(7);
	modelIndices.Add(3);
	modelIndices.Add(6);
	modelIndices.Add(3);
	modelIndices.Add(2);

	modelIndices.Add(4);
	modelIndices.Add(0);
	modelIndices.Add(3);
	modelIndices.Add(4);
	modelIndices.Add(3);
	modelIndices.Add(7);

	modelIndices.Add(6);
	modelIndices.Add(2);
	modelIndices.Add(1);
	modelIndices.Add(6);
	modelIndices.Add(1);
	modelIndices.Add(5);

	// VERTEX BUFFER
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(SVertexDataCube) * modelVertices.Size();
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = &modelVertices[0];
	HRESULT result;
	result = FRAMEWORK->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
	CHECK_RESULT(result, "Failed to create vertexbuffer.");

	// INDEX BUFFER
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(int) * modelIndices.Size();
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = &modelIndices[0];
	result = FRAMEWORK->GetDevice()->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
	CHECK_RESULT(result, "Failed to create indexbuffer.");
}


void CSkybox::UpdateCbuffer(const CU::Camera& aCamera)
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	//VertexCBuffer
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));

	CU::Matrix44f updatedSpace;
	//updatedSpace = /*aCamera.GetTransformation()*/;
	updatedSpace.SetPosition(aCamera.GetPosition());

	DEVICE_CONTEXT->Map(myVSBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, &updatedSpace, sizeof(CU::Matrix44f));
	DEVICE_CONTEXT->Unmap(myVSBuffer, 0);
	DEVICE_CONTEXT->VSSetConstantBuffers(1, 1, &myVSBuffer);


	CU::Vector4f updatedCameraPos(aCamera.GetPosition().x, aCamera.GetPosition().y, aCamera.GetPosition().z, 1.f);
	ZeroMemory(&mappedSubresource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	DEVICE_CONTEXT->Map(myPSBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, &updatedCameraPos, sizeof(CU::Vector4f));
	DEVICE_CONTEXT->Unmap(myPSBuffer, 0);
	DEVICE_CONTEXT->PSSetConstantBuffers(1, 1, &myPSBuffer);
}

#include "stdafx.h"
#include "LightModel.h"
#include "FBXLoader.h"
#include "ConstBufferTemplate.h"

CLightModel::CLightModel()
{
	myEffect = nullptr;
	myVertexBuffer = nullptr;
	myIndexBuffer = nullptr;
	myWorldSpaceBuffer = nullptr;
	myVertexCount = 0;
	myVertexSize = 0;
	myIndexCount = 0;
}

CLightModel::~CLightModel()
{
	SAFE_DELETE(myEffect);

	SAFE_RELEASE(myIndexBuffer);
	SAFE_RELEASE(myVertexBuffer);
	SAFE_RELEASE(myWorldSpaceBuffer);
}

void CLightModel::Init(CEffect* aEffect, const CLoaderMesh* aLoadedMesh)
{
	myEffect = aEffect;
	CU::Matrix44f someMatrix;
	myWorldSpaceBuffer = BSR::CreateCBuffer<CU::Matrix44f>(&someMatrix);

	myVertexCount = aLoadedMesh->myVertexCount;
	myVertexSize = aLoadedMesh->myVertexBufferSize;
	myIndexCount = (unsigned int)aLoadedMesh->myIndexes.size();

	// VERTEX BUFFER
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = aLoadedMesh->myVertexCount * myVertexSize;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexData;
	ZeroMemory(&vertexData, sizeof(vertexData));
	vertexData.pSysMem = aLoadedMesh->myVerticies;
	HRESULT result;
	result = DEVICE->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
	CHECK_RESULT(result, "Failed to create vertexbuffer.");


	// INDEX BUFFER
	D3D11_BUFFER_DESC indexBufferDesc;
	ZeroMemory(&indexBufferDesc, sizeof(indexBufferDesc));
	indexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	indexBufferDesc.ByteWidth = sizeof(int) * myIndexCount;
	indexBufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexBufferDesc.CPUAccessFlags = 0;
	indexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = &aLoadedMesh->myIndexes[0];
	result = DEVICE->CreateBuffer(&indexBufferDesc, &indexData, &myIndexBuffer);
	CHECK_RESULT(result, "Failed to create indexbuffer.");
}

void CLightModel::Render(const CU::Matrix44f& aTransformation)
{
	myEffect->Activate();

	BSR::UpdateCBuffer(myWorldSpaceBuffer, &aTransformation);
	DEVICE_CONTEXT->VSSetConstantBuffers(1, 1, &myWorldSpaceBuffer);

	UINT stride = myVertexSize;
	UINT offset = 0;

	DEVICE_CONTEXT->IASetVertexBuffers(0, 1, &myVertexBuffer, &stride, &offset);
	DEVICE_CONTEXT->IASetIndexBuffer(myIndexBuffer, DXGI_FORMAT_R32_UINT, 0);
	DEVICE_CONTEXT->DrawIndexed(myIndexCount, 0, 0);
}

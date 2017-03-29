#include "stdafx.h"
#include "Sprite.h"
#include "SpriteInstance.h"

#include "Engine.h"
#include "DXFramework.h"
#include "ShaderManager.h"
#include "Effect.h"
#include "Surface.h"

#include "ConstBufferTemplate.h"
#include "../CommonUtilities/vector2.h"
#include "../CommonUtilities/StringHelper.h"
#include "EModelBluePrint.h"

struct SVertexConstantBufferType
{
	CU::Vector2f position;
	CU::Vector2f size;
	CU::Vector2f pivot;
	CU::Vector2f TRASH;
	CU::Vector4f rect;
	CU::Vector4f color;
};

CSprite::CSprite()
	: myEffect(nullptr)
	, myVertexConstantBuffer(nullptr)
	, myVertexBuffer(nullptr)
	, mySurface(nullptr)
{
}

CSprite::~CSprite()
{
	SAFE_DELETE(myEffect);
	SAFE_DELETE(mySurface);
	SAFE_RELEASE(myVertexBuffer);
	SAFE_RELEASE(myVertexConstantBuffer);
}

CSprite& CSprite::operator=(const CSprite& aSprite)
{
	SAFE_DELETE(myEffect);
	myEffect = new CEffect(*aSprite.myEffect);
	
	SAFE_RELEASE(myVertexConstantBuffer);
	myVertexConstantBuffer = aSprite.myVertexConstantBuffer;
	myVertexConstantBuffer->AddRef();
	
	SAFE_RELEASE(myVertexBuffer);
	myVertexBuffer = aSprite.myVertexBuffer;
	myVertexBuffer->AddRef();

	SAFE_DELETE(mySurface);
	mySurface = new CSurface(*aSprite.mySurface);

	return *this;
}

CSprite& CSprite::operator=(CSprite&& aSprite)
{
	SAFE_DELETE(myEffect);
	myEffect = aSprite.myEffect;
	aSprite.myEffect = nullptr;

	SAFE_RELEASE(myVertexConstantBuffer);
	myVertexConstantBuffer = aSprite.myVertexConstantBuffer;
	aSprite.myVertexConstantBuffer = nullptr;

	SAFE_RELEASE(myVertexBuffer);
	myVertexBuffer = aSprite.myVertexBuffer;
	aSprite.myVertexBuffer = nullptr;

	SAFE_DELETE(mySurface);
	mySurface = aSprite.mySurface;
	aSprite.mySurface = nullptr;

	return *this;
}

void CSprite::Init(const char* aTexturePath)
{
	CreateEffect();
	CreateSurface(aTexturePath);

	bool result = InitBuffers();
	CHECK_BOOL_RESULT(result, "Failed To init Vertex buffer.");
}

bool CSprite::InitBuffers()
{
	/*CU::Vector4f topLeft  = { 0.0f	, 1.0f  , 0.5f, 1.0f };
	CU::Vector4f botLeft  = { 1.0f	, 0.0f	, 0.5f, 1.0f };
	CU::Vector4f topRight = { 0.0f	, 0.0f  , 0.5f, 1.0f };
	CU::Vector4f botRight = { 0.0f	, 1.0f	, 0.5f, 1.0f };*/

	CU::Vector4f vertices[6] =
	{
		CU::Vector4f(0.0f, 0.0f, -0.50f, 1.0f),
		CU::Vector4f(0.0f, 1.0f, -0.50f, 1.0f),
		CU::Vector4f(1.0f, 1.0f, -0.50f, 1.0f),
		CU::Vector4f(0.0f, 0.0f, -0.50f, 1.0f),
		CU::Vector4f(1.0f, 1.0f, -0.50f, 1.0f),
		CU::Vector4f(1.0f, 0.0f, -0.50f, 1.0f)
		//topLeft, botLeft, botRight, botRight, topRight, topLeft
	};

	// VERTEX BUFFER
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(vertices);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = vertices;
	HRESULT result = FRAMEWORK->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &myVertexBuffer);
	CHECK_RESULT(result, "Failed to create vertexbuffer.");

	SVertexConstantBufferType bufferData;
	myVertexConstantBuffer = BSR::CreateCBuffer<SVertexConstantBufferType>(&bufferData);

	return SUCCEEDED(result);
}

void CSprite::Render(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const CU::Vector2f& aPivot, const CU::Vector4f& aRect, const CU::Vector4f& aColor)
{
	myEffect->Activate();
	mySurface->Activate();
	
	UpdateAndSetVertexConstantBuffer(aPosition, aSize, aPivot, aRect, aColor);

	UINT stride = sizeof(CU::Vector4f);
	UINT offset = 0;
	FRAMEWORK->GetDeviceContext()->IASetVertexBuffers(0, 1, &myVertexBuffer, &stride, &offset);
	FRAMEWORK->GetDeviceContext()->Draw(6, 0);
}

CU::Vector2f CSprite::GetTextureSizeFloat() const
{
	if (mySurface != nullptr)
	{
		return mySurface->GetTextureSizeFloat();
	}

	return CU::Vector2f::Zero;
}

void CSprite::UpdateAndSetVertexConstantBuffer(const CU::Vector2f& aPosition, const CU::Vector2f& aSize,
	const CU::Vector2f& aPivot, const CU::Vector4f& aRect, const CU::Vector4f& aColor)
{
	ID3D11DeviceContext& context = *CEngine::GetInstance()->GetFramework()->GetDeviceContext();

	SVertexConstantBufferType cbufferStruct = {};
	cbufferStruct.position = aPosition;
	cbufferStruct.size = aSize;
	cbufferStruct.pivot = aPivot;
	cbufferStruct.TRASH = { 0.f,0.f };
	cbufferStruct.rect = aRect;
	cbufferStruct.color = aColor;

	D3D11_MAPPED_SUBRESOURCE mappedSubResource = {};

	context.Map(myVertexConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubResource);
	memcpy(mappedSubResource.pData, &cbufferStruct, sizeof(SVertexConstantBufferType));
	context.Unmap(myVertexConstantBuffer, 0);

	context.VSSetConstantBuffers(1, 1, &myVertexConstantBuffer);


}

void CSprite::CreateEffect()
{
	unsigned int shaderBluePrint = EModelBluePrint_Sprite2D;
	ID3D11VertexShader* vertexShader = CEngine::GetInstance()->GetShaderManager()->LoadVertexShader(L"Shaders/sprite_shader.fx", shaderBluePrint);
	ID3D11PixelShader* pixelShader = CEngine::GetInstance()->GetShaderManager()->LoadPixelShader(L"Shaders/sprite_shader.fx", shaderBluePrint);
	ID3D11InputLayout* inputLayout = CEngine::GetInstance()->GetShaderManager()->LoadInputLayout(L"Shaders/sprite_shader.fx", shaderBluePrint);

	myEffect = new CEffect(vertexShader, pixelShader, nullptr, inputLayout, D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void CSprite::CreateSurface(const char* aTexturePath)
{
	CU::GrowingArray<const wchar_t*> texturePath(1);
	wchar_t buffer[1024];
	texturePath.Add(CU::CharToWChar(buffer, aTexturePath));
	mySurface = new CSurface(texturePath);
}

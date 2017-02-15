#include "stdafx.h"
#include "LineDrawer.h"

#include "Engine.h"
#include "DXFramework.h"
#include "Effect.h"
#include "EModelBluePrint.h"
#include "ShaderManager.h"

#define LINE_2D_VERTEX_COUNT (myLines2D.Size() * 2)

CLineDrawer::CLineDrawer()
	: myLines2D(8)
	, myLineHandles2D(8)
	, myVertexBuffer2D(nullptr)
	, myEffect3D(nullptr)
	, myVertexBuffer3D(nullptr)
{
	CShaderManager* shaderManager = CEngine::GetInstance()->GetShaderManager();
	if (shaderManager == nullptr) return;

	const unsigned int shaderBluePrint2d = EModelBluePrint::EModelBluePrint_Position;

	ID3D11VertexShader* vertexShader2d = shaderManager->LoadVertexShader(L"Shaders/line_shader.fx", shaderBluePrint2d);
	ID3D11PixelShader* pixelShader2d = shaderManager->LoadPixelShader(L"Shaders/line_shader.fx", shaderBluePrint2d);
	ID3D11InputLayout* inputLayout2d = shaderManager->LoadInputLayout(L"Shaders/line_shader.fx", shaderBluePrint2d);

	myEffect2D = new CEffect(vertexShader2d, pixelShader2d, nullptr, inputLayout2d, D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	if (std::ifstream("Shaders/line_shader_3d.fx").good())
	{
		const unsigned int shaderBluePrint3d = EModelBluePrint::EModelBluePrint_Position;

		ID3D11VertexShader* vertexShader3d = shaderManager->LoadVertexShader(L"Shaders/line_shader_3d.fx", shaderBluePrint3d);
		ID3D11PixelShader* pixelShader3d = shaderManager->LoadPixelShader(L"Shaders/line_shader_3d.fx", shaderBluePrint3d);
		ID3D11InputLayout* inputLayout3d = shaderManager->LoadInputLayout(L"Shaders/line_shader_3d.fx", shaderBluePrint3d);

		myEffect3D = new CEffect(vertexShader3d, pixelShader3d, nullptr, inputLayout3d, D3D_PRIMITIVE_TOPOLOGY::D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
	}

	CreateVertexBuffers();
}

CLineDrawer::~CLineDrawer()
{
	SAFE_DELETE(myEffect2D);
}

void CLineDrawer::Render()
{
#ifndef _RETAIL_BUILD
	RenderLines2D();
#endif // !_RETAIL_BUILD
}

LineHandle CLineDrawer::AddLine(const SLine2D& aLine2D)
{
	if (myLines2D.Size() >= ourMaxNumberOfLines)
	{
		DL_ASSERT("To many lines for the system to handle! talk to carl or find out how to fix");
		return static_cast<LineHandle>(-1);
	}

	LineIndex lineIndex = myLines2D.Size();
	myLines2D.Add(aLine2D);

	LineHandle lineHandle = myLineHandles2D.Size();
	myLineHandles2D.Add(lineIndex);

	return lineHandle;
}

LineHandle CLineDrawer::AddLine(const CU::Vector2f& aStart, const CU::Vector2f& aEnd)
{
	SLine2D line2D;
	line2D.myStart.Set(aStart.x, aStart.y, 0.f, 1.f);
	line2D.myEnd.Set(aEnd.x, aEnd.y, 0.f, 1.f);
	return AddLine(line2D);
}

void CLineDrawer::AddQuad(const CU::Vector2f& aMinPoint, const CU::Vector2f& aMaxPoint, LineHandle aLineHandlesOut[])
{
	if (aLineHandlesOut == nullptr)
	{
		DL_ASSERT("aLineHandlesOut cannot be NULL, because then you cannot destroy them!\nExample: Declare \"LineHandle lineHandleQuad[4]\" and send as argument");
		return;
	}

	aLineHandlesOut[0] = AddLine(CU::Vector2f(aMinPoint.x, aMinPoint.y), CU::Vector2f(aMaxPoint.x, aMinPoint.y));
	aLineHandlesOut[1] = AddLine(CU::Vector2f(aMaxPoint.x, aMinPoint.y), CU::Vector2f(aMaxPoint.x, aMaxPoint.y));
	aLineHandlesOut[2] = AddLine(CU::Vector2f(aMaxPoint.x, aMaxPoint.y), CU::Vector2f(aMinPoint.x, aMaxPoint.y));
	aLineHandlesOut[3] = AddLine(CU::Vector2f(aMinPoint.x, aMaxPoint.y), CU::Vector2f(aMinPoint.x, aMinPoint.y));
}

void CLineDrawer::RemoveLine2D(const LineHandle aLineHandle)
{
	LineIndex lineIndex = myLineHandles2D[aLineHandle];
	myLineHandles2D[aLineHandle] = NullIndex;

	myLines2D.RemoveCyclicAtIndex(lineIndex);

	LineIndex movedIndex = myLines2D.Size();
	if (lineIndex != movedIndex)
	{
		LineHandle movedHandle = myLineHandles2D.Find(movedIndex);
		myLineHandles2D[movedHandle] = lineIndex;
	}

	if (movedIndex == 0)
	{
		myLineHandles2D.RemoveAll();
	}
}

void CLineDrawer::RenderLineChunk(const CU::GrowingArray<char, unsigned int, false>& aLineChunk)
{
	if (myVertexBuffer3D == nullptr) return;
	if (aLineChunk.Size() <= 0) return;

	ID3D11DeviceContext* context = DEVICE_CONTEXT;
	if (context == nullptr) return;
	
	myEffect3D->Activate();

	UpdateVertexBuffer3D(aLineChunk, *context);

	UINT stride = sizeof(float) * 4u;
	UINT offset = 0;

	context->IASetVertexBuffers(0, 1, &myVertexBuffer3D, &stride, &offset);
	context->Draw(static_cast<UINT>(aLineChunk.Size()) / stride, 0u);
}

void CLineDrawer::CreateVertexBuffers()
{
	ID3D11Device* device = DEVICE;
	if (device == nullptr) return;

	//2D
	{
		D3D11_BUFFER_DESC vertexBufferDesc = {};

		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(SLine2D) * ourMaxNumberOfLines;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;

		HRESULT result = device->CreateBuffer(&vertexBufferDesc, nullptr, &myVertexBuffer2D);
		CHECK_RESULT(result, "Failed to create vertex buffer.");
	}

	//3D
	if (std::ifstream("Shaders/line_shader.fx").good())
	{
		D3D11_BUFFER_DESC vertexBufferDesc = {};

		vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		vertexBufferDesc.ByteWidth = sizeof(SLine2D) * ourMaxNumberOfLines;
		vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		vertexBufferDesc.MiscFlags = 0;

		HRESULT result = device->CreateBuffer(&vertexBufferDesc, nullptr, &myVertexBuffer3D);
		CHECK_RESULT(result, "Failed to create vertex buffer.");
	}
}

void CLineDrawer::RenderLines2D()
{
	if (myVertexBuffer2D != nullptr)
	{
		ID3D11DeviceContext* context = DEVICE_CONTEXT;
		if (context != nullptr)
		{
			myEffect2D->Activate();

			UpdateVertexBuffer2D();

			UINT stride = sizeof(SLine2D) / 2;
			UINT offset = 0;

			context->IASetVertexBuffers(0, 1, &myVertexBuffer2D, &stride, &offset);
			context->Draw(LINE_2D_VERTEX_COUNT, 0);
		}
	}
}

void CLineDrawer::UpdateVertexBuffer2D()
{
	if (myLines2D.Size() > 0)
	{
		ID3D11DeviceContext* context = CEngine::GetInstance()->GetFramework()->GetDeviceContext();
		if (context != nullptr)
		{
			D3D11_MAPPED_SUBRESOURCE mappedSubresource = {};

			unsigned int bufferSize = sizeof(SLine2D) / 2;
			if (myLines2D.Size() > ourMaxNumberOfLines)
			{
				bufferSize *= ourMaxNumberOfLines * 2;
			}
			else
			{
				bufferSize *= LINE_2D_VERTEX_COUNT;
			}

			context->Map(myVertexBuffer2D, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
			memcpy(mappedSubresource.pData, &myLines2D[0], bufferSize);
			context->Unmap(myVertexBuffer2D, 0);
		}
	}
}

void CLineDrawer::UpdateVertexBuffer3D(const CU::GrowingArray<char, unsigned int, false>& aLineChunk, ID3D11DeviceContext& aContext)
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource = {};
	aContext.Map(myVertexBuffer3D, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, aLineChunk.AsVoidPointer(), min(aLineChunk.ByteSize(), ourMaxNumberOfLines * sizeof(float) * 4u * 2u));
	aContext.Unmap(myVertexBuffer3D, 0);
}

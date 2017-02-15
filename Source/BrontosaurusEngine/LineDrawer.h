#pragma once

#include "../CommonUtilities/vector2.h"
#include "../CommonUtilities/vector4.h"

using LineHandle = int;
using LineIndex = LineHandle;
struct ID3D11Buffer;
struct ID3D11DeviceContext;
class CEffect;

class CLineDrawer
{
	struct SLine2D
	{
		CU::Vector4f myStart;
		CU::Vector4f myEnd;
	};

public:
	CLineDrawer();
	~CLineDrawer();

	void Render();

	LineHandle AddLine(const CU::Vector2f& aStart, const CU::Vector2f& aEnd);
	void AddQuad(const CU::Vector2f& aMinPoint, const CU::Vector2f& aMaxPoint, LineHandle aLineHandlesOut[]);
	void RemoveLine2D(const LineHandle aLineHandle);

	void RenderLineChunk(const CU::GrowingArray<char, unsigned int, false>& aLineChunk);

private:
	LineHandle AddLine(const SLine2D& aLine2D);

	void CreateVertexBuffers();
	void RenderLines2D();
	void UpdateVertexBuffer2D();
	void UpdateVertexBuffer3D(const CU::GrowingArray<char, unsigned int, false>& aLineChunk, ID3D11DeviceContext& aContext);

	CU::GrowingArray<SLine2D, LineIndex, false> myLines2D;
	CU::GrowingArray<LineIndex, LineHandle, false> myLineHandles2D;

	CEffect* myEffect2D;
	ID3D11Buffer* myVertexBuffer2D;

	CEffect* myEffect3D;
	ID3D11Buffer* myVertexBuffer3D;

	static const LineIndex NullIndex = -2;
	static const unsigned int ourLine2DVertexCount = 2;
	static const unsigned int ourMaxNumberOfLines = USHRT_MAX;
};

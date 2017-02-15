#pragma once
#include "DXFramework.h"
#include "Engine.h" 

namespace BSR
{
	template<typename TYPE>
	ID3D11Buffer* CreateCBuffer(TYPE* aData)
	{
		D3D11_BUFFER_DESC constBufferDesc;
		ZeroMemory(&constBufferDesc, sizeof(constBufferDesc));
		constBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		constBufferDesc.Usage = D3D11_USAGE_DYNAMIC; // KANSKE DEFAULT?// DYNAMIC ÄR BRA FÖR DET KAN BARA LÄSAS AV GPU O SKRIVAS AV CPU BALLZ
		constBufferDesc.ByteWidth = sizeof(TYPE);
		constBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		constBufferDesc.MiscFlags = 0;
		constBufferDesc.StructureByteStride = 0;

		ID3D11Buffer* constBuffer;
		D3D11_SUBRESOURCE_DATA subResourceData;
		ZeroMemory(&subResourceData, sizeof(subResourceData));
		subResourceData.pSysMem = &aData;
		HRESULT result;
		result = CEngine::GetInstance()->GetFramework()->GetDevice()->CreateBuffer(&constBufferDesc, &subResourceData, &constBuffer);
		CHECK_RESULT(result, "Failed to create constantbuffer.");

		return constBuffer;
	}
}
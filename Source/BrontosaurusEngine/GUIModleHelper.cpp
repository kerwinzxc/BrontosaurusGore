#include "stdafx.h"
#include "GUIModleHelper.h"

#include "Engine.h"
#include "ShaderManager.h"
#include "FBXLoader.h"
#include "Effect.h"
#include "Surface.h"

#include "../CommonUtilities/StringHelper.h"

namespace GUIModleHelper
{
	CEffect* CreateEffect(const CLoaderMesh* aLoaderMesh)
	{
		struct ID3D11VertexShader* vertexShader = CEngine::GetInstance()->GetShaderManager()->LoadVertexShader(L"Shaders/gui_shader.fx", aLoaderMesh->myShaderType);
		struct ID3D11InputLayout* inputLayout = CEngine::GetInstance()->GetShaderManager()->LoadInputLayout(L"Shaders/gui_shader.fx", aLoaderMesh->myShaderType);
		struct ID3D11PixelShader* pixelShader = CEngine::GetInstance()->GetShaderManager()->LoadPixelShader(L"Shaders/gui_shader.fx", aLoaderMesh->myShaderType);

		CEffect* effect = new CEffect(vertexShader, pixelShader, nullptr, inputLayout, D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		//SPixelConstantBuffer pixelConstantBufferStruct = {};
		//ID3D11Buffer* pixelConstantBuffer = BSR::CreateCBuffer(&pixelConstantBufferStruct);
		//effect->AddConstantBuffer(CEffect::eShaderStage::ePixel, pixelConstantBuffer);
		return effect;
	}

	CSurface* CreateSurface(const char* aTexturePath)
	{
		wchar_t buffer[512];
		CU::GrowingArray<const wchar_t*> texturePaths(1);
		texturePaths.Add(CU::CharToWChar(buffer, aTexturePath));
		CSurface* surface = new CSurface(texturePaths);

		return surface;
	}

	CSurface* CreateSurface(const CU::GrowingArray<std::string>& aTexturePaths)
	{
		wchar_t buffer[32][512];
		CU::GrowingArray<const wchar_t*> texturePaths(10);
		for (unsigned int i = 0; i < aTexturePaths.Size(); ++i)
		{
			texturePaths.Add(CU::CharToWChar(buffer[i], aTexturePaths[i].c_str()));
		}

		CSurface* surface = new CSurface(texturePaths);

		return surface;
	}
}

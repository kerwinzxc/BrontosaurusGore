#include "stdafx.h"
#include "ShaderManager.h"
#include "VertexStructs.h"
#include "EffectHelper.h"
#include "Engine.h"

CShaderManager::CShaderManager() //precompiled shaders later
{
}

CShaderManager::~CShaderManager()
{
	for (auto it = myVertexShaders.begin(); it != myVertexShaders.end(); it++)
	{
		for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
		{
			SAFE_RELEASE(it2->second);
		}
	}

	for (auto it = myPixelShaders.begin(); it != myPixelShaders.end(); it++)
	{
		for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
		{
			SAFE_RELEASE(it2->second);
		}
	}

	for (auto it = myGeometryShaders.begin(); it != myGeometryShaders.end(); it++)
	{
		for (auto it2 = it->second.begin(); it2 != it->second.end(); ++it2)
		{
			SAFE_RELEASE(it2->second);
		}
	}

	for (auto it = myInputLayouts.begin(); it != myInputLayouts.end(); it++)
	{
		SAFE_RELEASE(it->second);
	}
}

ID3D11VertexShader * CShaderManager::LoadVertexShader(std::wstring aShaderPath,  unsigned int aShaderBlueprint)
{
	if (myVertexShaders.find(aShaderPath) == myVertexShaders.end())
	{
		std::unordered_map<unsigned int, ID3D11VertexShader*> vertexMap;
		myVertexShaders[aShaderPath] = vertexMap;
	}

	if (myVertexShaders[aShaderPath].find(aShaderBlueprint) == myVertexShaders[aShaderPath].end())
	{
		ID3D11VertexShader* newShader = CreateVertexShader(aShaderPath, aShaderBlueprint);
		myVertexShaders[aShaderPath][aShaderBlueprint] = newShader;
	}

	return myVertexShaders[aShaderPath][aShaderBlueprint];
}

ID3D11PixelShader* CShaderManager::LoadPixelShader(std::wstring aShaderPath, unsigned int aShaderBlueprint)
{
	if (myPixelShaders.find(aShaderPath) == myPixelShaders.end())
	{
		std::unordered_map<unsigned int, ID3D11PixelShader*> pixelMap;
		myPixelShaders[aShaderPath] = pixelMap;
	}

	if (myPixelShaders[aShaderPath].find(aShaderBlueprint) == myPixelShaders[aShaderPath].end())
	{
		ID3D11PixelShader* newShader = CreatePixelShader(aShaderPath, aShaderBlueprint);
		myPixelShaders[aShaderPath][aShaderBlueprint] = newShader;
	}

	return myPixelShaders[aShaderPath][aShaderBlueprint];
}

ID3D11GeometryShader * CShaderManager::LoadGeometryShader(std::wstring aShaderPath, unsigned int aShaderBlueprint)
{
	if (myGeometryShaders.find(aShaderPath) == myGeometryShaders.end())
	{
		std::unordered_map<unsigned int, ID3D11GeometryShader*> geometryMap;
		myGeometryShaders[aShaderPath] = geometryMap;
	}

	if (myGeometryShaders[aShaderPath].find(aShaderBlueprint) == myGeometryShaders[aShaderPath].end())
	{
		ID3D11GeometryShader* newShader = CreateGeometryShader(aShaderPath, aShaderBlueprint);
		myGeometryShaders[aShaderPath][aShaderBlueprint] = newShader;
	}

	return myGeometryShaders[aShaderPath][aShaderBlueprint];

}

ID3D11InputLayout * CShaderManager::LoadInputLayout(std::wstring aShaderPath, unsigned int aShaderBlueprint)
{
	if (myInputLayouts.find(aShaderBlueprint) == myInputLayouts.end())
	{
		LoadVertexShader(aShaderPath, aShaderBlueprint);
	}

	return myInputLayouts.at(aShaderBlueprint);
}

ID3D11VertexShader * CShaderManager::CreateVertexShader(const std::wstring& aString, unsigned int aDataFlags)
{
//#define USE_PRECOMPILED_SHADERS
#ifdef USE_PRECOMPILED_SHADERS

	ID3D11Device* device = DEVICE;
	if (device == nullptr) return nullptr;

	std::wstring compiledShaderPath(aString);
	compiledShaderPath -= L".fx";
	compiledShaderPath += std::to_wstring(aDataFlags) += L".fxc";
	std::ifstream shaderByteCode(compiledShaderPath, std::ios::binary);
	bool precompiledIsNewerThanSourceCode = false;
	bool usePreCompiledShader = shaderByteCode.good() && precompiledIsNewerThanSourceCode == true;

	if (usePreCompiledShader == true)
	{
		std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
#ifdef _DEBUG
		std::string shaderFunction = "VS_";
		shaderFunction +=
#else
		(void)
#endif // _DEBUG
				GetInputLayoutType(aDataFlags, inputLayoutDesc);

		shaderByteCode.seekg(0, shaderByteCode.end);
		long long bytesOfFile = shaderByteCode.tellg();
		
		CU::GrowingArray<char> vertexBlob(bytesOfFile + 1); //include room for null terminator '\0' ??
		vertexBlob.Resize(bytesOfFile);
		shaderByteCode.seekg(0, shaderByteCode.beg);
		shaderByteCode.read(vertexBlob.AsPointer(), bytesOfFile);
		vertexBlob.Add('\0');

		ID3D11VertexShader* vertexShader = nullptr;
		HRESULT result = device->CreateVertexShader(vertexBlob.AsVoidPointer(), vertexBlob.ByteSize(), nullptr, &vertexShader);
		CHECK_RESULT(result, "Failed to create Vertex Shader.");
		
		ID3D11InputLayout* inputLayout = nullptr;
		inputLayout = CreateInputLayout(inputLayout, vertexBlob.AsVoidPointer(), vertexBlob.ByteSize(), inputLayoutDesc);
		myInputLayouts[aDataFlags] = inputLayout;

		return vertexShader;
	}

	shaderByteCode.close();

#endif // USE_PRECOMPILED_SHADERS

	return CompileVertexShader(aString, aDataFlags);
}

ID3D11PixelShader * CShaderManager::CreatePixelShader(const std::wstring& aString, unsigned int aDataFlags)
{
	return CompilePixelShader(aString, aDataFlags);
}

ID3D11GeometryShader * CShaderManager::CreateGeometryShader(const std::wstring& aString, unsigned int aDataFlags)
{
	return CompileGeometryShader(aString, aDataFlags);
}

ID3D11VertexShader* CShaderManager::CompileVertexShader(std::wstring aString, unsigned int aDataFlags)
{
	ID3D11VertexShader* vertexShader;
	ID3D10Blob* vertexBlob = nullptr;

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	std::string shaderFunction = "VS_";

	shaderFunction += GetInputLayoutType(aDataFlags, inputLayoutDesc, (aDataFlags & EModelBluePrint::EModelBluePrint_Instance) > 0);
	
	std::string errorMsg;
	bool bresult;
	HRESULT result;

	bresult = EffectHelper::CompileShader(aString.c_str(), shaderFunction.c_str() , "vs_5_0", vertexBlob, errorMsg);
	CHECK_BOOL_RESULT(bresult, errorMsg.c_str());
	if (bresult == false)
	{
		DL_FATAL_ERROR("Failed to compile shader %s with file path %S", shaderFunction.c_str(), aString.c_str());
	}

#ifdef USE_PRECOMPILED_SHADERS

	std::wstring compiledShaderPath(aString);
	compiledShaderPath -= L".fx";
	compiledShaderPath += std::to_wstring(aDataFlags) += L".fxc";
	std::ofstream compiledShader(compiledShaderPath, std::ios::binary);
	compiledShader.seekp(0);
	compiledShader.write(static_cast<char*>(vertexBlob->GetBufferPointer()), vertexBlob->GetBufferSize());

#endif // USE_PRECOMPILED_SHADERS

	result = CEngine::GetInstance()->GetFramework()->GetDevice()->CreateVertexShader(vertexBlob->GetBufferPointer(), vertexBlob->GetBufferSize(), NULL, &vertexShader);
	CHECK_RESULT(result, "Failed to create Vertex Shader.");

	
	myInputLayouts[aDataFlags] = CreateInputLayout(nullptr, vertexBlob, inputLayoutDesc);

	vertexBlob->Release();
	return vertexShader;
}

ID3D11PixelShader* CShaderManager::CompilePixelShader(std::wstring aString, unsigned int aDataFlags)
{
	ID3D10Blob* pixelBlob = nullptr;
	ID3D11PixelShader* pixelShader = nullptr;

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	std::string shaderFunction = "PS_";
	shaderFunction += GetInputLayoutType(aDataFlags, inputLayoutDesc);

	std::string errorMsg;
	bool bresult;

	bresult = EffectHelper::CompileShader(aString.c_str(), shaderFunction.c_str(), "ps_5_0", pixelBlob, errorMsg);
	CHECK_BOOL_RESULT(bresult, errorMsg.c_str());

	HRESULT result;
	result = CEngine::GetInstance()->GetFramework()->GetDevice()->CreatePixelShader(pixelBlob->GetBufferPointer(), pixelBlob->GetBufferSize(), NULL, &pixelShader);
	CHECK_RESULT(result, "Failed to create Pixel Shader.");

	pixelBlob->Release();
	return pixelShader;
}

ID3D11GeometryShader * CShaderManager::CompileGeometryShader(std::wstring aString, unsigned int aDataFlags)
{
	ID3D10Blob* geometryBlob = nullptr;
	ID3D11GeometryShader* geometryShader;

	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	std::string shaderFunction = "GS_";
	shaderFunction += GetInputLayoutType(aDataFlags, inputLayoutDesc);

	std::string errorMsg;
	bool bresult;

	bresult = EffectHelper::CompileShader(aString.c_str(), shaderFunction.c_str(), "gs_5_0", geometryBlob, errorMsg);
	CHECK_BOOL_RESULT(bresult, errorMsg.c_str());

	HRESULT result;
	result = FRAMEWORK->GetDevice()->CreateGeometryShader(geometryBlob->GetBufferPointer(), geometryBlob->GetBufferSize(), NULL, &geometryShader);

	geometryBlob->Release();
	return geometryShader;
}

//returns string and sets the layout. eehehe
std::string CShaderManager::GetInputLayoutType(unsigned int aShaderBlueprint, std::vector<D3D11_INPUT_ELEMENT_DESC>& aInputLayout, const bool aInstanced)
{
	std::string layoutName = "";

	if (aShaderBlueprint & EModelBluePrint::EModelBluePrint_Position)
	{
		layoutName += "Pos";
		EffectHelper::CreateLayout(aInputLayout, "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	}
	else if (aShaderBlueprint & EModelBluePrint::EModelBluePrint_Sprite2D)
	{
		layoutName += "Sprite";
		EffectHelper::CreateLayout(aInputLayout, "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);

		return layoutName;
	}
	else if (aShaderBlueprint & EModelBluePrint::EModelBluePrint_Text2D)
	{
		layoutName += "Text";
		EffectHelper::CreateLayout(aInputLayout, "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
		EffectHelper::CreateLayout(aInputLayout, "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_INPUT_PER_VERTEX_DATA, 0);
		EffectHelper::CreateLayout(aInputLayout, "SCALE", 0, DXGI_FORMAT_R32G32_FLOAT, 1, D3D11_INPUT_PER_VERTEX_DATA, 0);
		return layoutName;
	}
	else if (aShaderBlueprint & EModelBluePrint::EModelBluePrint_Bar)
	{
		layoutName += "Bar";
		EffectHelper::CreateLayout(aInputLayout, "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
		return layoutName;
	}
	else if (aShaderBlueprint & EModelBluePrint::EModelBluePrint_Fire)
	{
		layoutName += "Fire";
		EffectHelper::CreateLayout(aInputLayout, "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
		EffectHelper::CreateLayout(aInputLayout, "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
		return layoutName;
	}
	else
	{
		assert(false && "You need a position!");
	}
	if (aShaderBlueprint & EModelBluePrint::EModelBluePrint_Normal)
	{
		layoutName += "Norm";
		EffectHelper::CreateLayout(aInputLayout, "NORMALS", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	}
	if (aShaderBlueprint & EModelBluePrint::EModelBluePrint_BinormTan)
	{
		layoutName += "BinormTan";
		EffectHelper::CreateLayout(aInputLayout, "TANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
		EffectHelper::CreateLayout(aInputLayout, "BITANGENT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	}
	if (aShaderBlueprint & EModelBluePrint::EModelBluePrint_UV)
	{
		layoutName += "Tex";
		EffectHelper::CreateLayout(aInputLayout, "UV", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	}
	if (aShaderBlueprint & EModelBluePrint::EModelBluePrint_Size)
	{
		layoutName += "Size";
		EffectHelper::CreateLayout(aInputLayout, "SIZE", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	}
	if (aShaderBlueprint & EModelBluePrint::EModelBluePrint_Alpha)
	{
		layoutName += "Alpha";
		EffectHelper::CreateLayout(aInputLayout, "ALPHA", 0, DXGI_FORMAT_R32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	}
	if (aShaderBlueprint & EModelBluePrint::EModelBluePrint_Bones)
	{
		layoutName += "Bones";
		EffectHelper::CreateLayout(aInputLayout, "BONE", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
		EffectHelper::CreateLayout(aInputLayout, "WEIGHT", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	}
	if (aShaderBlueprint & EModelBluePrint::EModelBluePrint_Color)
	{
		layoutName += "Color";
		EffectHelper::CreateLayout(aInputLayout, "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, D3D11_INPUT_PER_VERTEX_DATA, 0);
	}
	if (aShaderBlueprint & EModelBluePrint_Instance && !(aShaderBlueprint & EModelBluePrint_Bones))
	{
		layoutName += "Instanced";
		EffectHelper::CreateLayout(aInputLayout, "TOWORLD", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_INPUT_PER_INSTANCE_DATA, 1, true);
		EffectHelper::CreateLayout(aInputLayout, "TOWORLD", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
		EffectHelper::CreateLayout(aInputLayout, "TOWORLD", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
		EffectHelper::CreateLayout(aInputLayout, "TOWORLD", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_INPUT_PER_INSTANCE_DATA, 1);

		EffectHelper::CreateLayout(aInputLayout, "TOWORLDLASTFRAME", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
		EffectHelper::CreateLayout(aInputLayout, "TOWORLDLASTFRAME", 1, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
		EffectHelper::CreateLayout(aInputLayout, "TOWORLDLASTFRAME", 2, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
		EffectHelper::CreateLayout(aInputLayout, "TOWORLDLASTFRAME", 3, DXGI_FORMAT_R32G32B32A32_FLOAT, 1, D3D11_INPUT_PER_INSTANCE_DATA, 1);
	}

	return layoutName;
}

ID3D11InputLayout* CShaderManager::CreateInputLayout(ID3D11InputLayout* /*aLayout*/, ID3D10Blob* aVertexBlob, std::vector<D3D11_INPUT_ELEMENT_DESC>& aInputLayout)
{
	return CreateInputLayout(nullptr, aVertexBlob->GetBufferPointer(), static_cast<unsigned int>(aVertexBlob->GetBufferSize()), aInputLayout);
}

ID3D11InputLayout * CShaderManager::CreateInputLayout(ID3D11InputLayout * /*aLayout*/, const void * aVertexBlobPointer, const unsigned int aVertexBufferSize, std::vector<D3D11_INPUT_ELEMENT_DESC>& aInputLayout)
{
	ID3D11InputLayout* inputLayout = nullptr;
	HRESULT result;
	result = CEngine::GetInstance()->GetFramework()->GetDevice()->CreateInputLayout(&aInputLayout[0], static_cast<UINT>(aInputLayout.size()), aVertexBlobPointer, aVertexBufferSize, &inputLayout);
	CHECK_RESULT(result, "Failed to create inputlayout.");
	return inputLayout;
}



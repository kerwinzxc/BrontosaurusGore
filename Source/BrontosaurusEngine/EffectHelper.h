#pragma once
#include <fstream>
#include <d3dcompiler.h>
#include <vector>
#include <string>
#include "DXFramework.h"
//#include <GrowingArray.h>

namespace EffectHelper
{
	static bool FileExists(const wchar_t *fileName)
	{
		std::ifstream infile(fileName);
		bool isGood = infile.good();
		infile.close();
		return isGood;
	}

	static bool CompileShader(const wchar_t* aShader, const char* aMainFunction, const char* aTarget, ID3D10Blob*& aCodeBlob, std::string& outError)
	{
		if (!FileExists(aShader))
		{
			std::wstring wShader = aShader;
			std::string shaderName(wShader.begin(), wShader.end());
			outError = "Shader not found: " + shaderName;
			return false;
		}
		HRESULT result;
		ID3D10Blob* errorMessageBlob = nullptr;
		UINT flags = 0;
		const D3D_SHADER_MACRO* globalShaderDefines = nullptr;
#if defined (DEBUG) || defined (_DEBUG)
		flags |= D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION | D3DCOMPILE_PREFER_FLOW_CONTROL;
		//const D3D_SHADER_MACRO debugShaderDefines[] = { "DEBUG" "1" }; ?????
		//globalShaderDefines = globalShaderDefines;
#endif
		result = D3DCompileFromFile(aShader, globalShaderDefines, D3D_COMPILE_STANDARD_FILE_INCLUDE, aMainFunction, aTarget, flags, 0, &aCodeBlob, &errorMessageBlob);
		if (FAILED(result))
		{
			outError = "Failed to compile shader.";
			OutputDebugStringA(outError.c_str());
		}

		LPVOID voidError = NULL;
		if (errorMessageBlob)
		{
			voidError = errorMessageBlob->GetBufferPointer();
			const char* errorMessage = static_cast<const char*>(voidError);

			outError = errorMessage;

			OutputDebugStringA(outError.c_str());
			return false;
		}
		return true;
	}

	static void CreateLayout(std::vector<D3D11_INPUT_ELEMENT_DESC>& anArray, LPCSTR aName, int aSemanticIndex, DXGI_FORMAT aFormat, int aInputSlot, D3D11_INPUT_CLASSIFICATION aClassification, int aInstanceStep, const bool aForceInputSlotZero = false)
	{
		D3D11_INPUT_ELEMENT_DESC polygonLayout;
		polygonLayout.SemanticName = aName;
		polygonLayout.SemanticIndex = aSemanticIndex;
		polygonLayout.Format = aFormat;
		polygonLayout.InputSlot = aInputSlot;
		polygonLayout.AlignedByteOffset = (anArray.size() > 0 && aForceInputSlotZero == false) ? D3D11_APPEND_ALIGNED_ELEMENT : 0;
		polygonLayout.InputSlotClass = aClassification;
		polygonLayout.InstanceDataStepRate = aInstanceStep;
		anArray.push_back(polygonLayout);
	}
}
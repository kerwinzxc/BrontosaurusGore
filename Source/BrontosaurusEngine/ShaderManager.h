#pragma once
#include <vector>
#include <unordered_map>
#include <d3dcommon.h>

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11GeometryShader;
struct ID3D11InputLayout;
struct D3D11_INPUT_ELEMENT_DESC;

class CShaderManager
{
public:
	CShaderManager();
	~CShaderManager();
	ID3D11VertexShader* LoadVertexShader(std::wstring aString, unsigned int aShaderBlueprint);
	ID3D11PixelShader* LoadPixelShader(std::wstring aString, unsigned int aShaderBlueprint);
	ID3D11GeometryShader* LoadGeometryShader(std::wstring aString, unsigned int aShaderBlueprint);
	ID3D11InputLayout * LoadInputLayout(std::wstring aString, unsigned int aShaderBlueprint);

private:
	ID3D11VertexShader * CreateVertexShader(const std::wstring& aString, unsigned int aDataFlags);
	ID3D11PixelShader * CreatePixelShader(const std::wstring& aString, unsigned int aDataFlags);
	ID3D11GeometryShader* CreateGeometryShader(const std::wstring& aString, unsigned int aDataFlags);

	ID3D11VertexShader * CompileVertexShader(std::wstring aString, unsigned int aDataFlags);
	ID3D11PixelShader * CompilePixelShader(std::wstring aString, unsigned int aDataFlags);
	ID3D11GeometryShader* CompileGeometryShader(std::wstring aString, unsigned int aDataFlags);
	ID3D11InputLayout* CreateInputLayout(ID3D11InputLayout * aLayout, ID3D10Blob * aVertexBlob, std::vector<D3D11_INPUT_ELEMENT_DESC>& aInputLayout);
	ID3D11InputLayout* CreateInputLayout(ID3D11InputLayout * aLayout, const void* aVertexBlobPointer, const unsigned int aVertexBufferSize, std::vector<D3D11_INPUT_ELEMENT_DESC>& aInputLayout);
	std::string GetInputLayoutType(unsigned int aShaderBlueprint, std::vector<D3D11_INPUT_ELEMENT_DESC>& aInputLayout, const bool aInstanced = false);
	void AddInstanceDataToLayout(unsigned int aShaderBlueprint, std::vector<D3D11_INPUT_ELEMENT_DESC>& aInputLayout);

private:
	std::unordered_map<std::wstring, std::unordered_map<unsigned int, ID3D11VertexShader*	>> myVertexShaders;
	std::unordered_map<std::wstring, std::unordered_map<unsigned int, ID3D11PixelShader*	>> myPixelShaders;
	std::unordered_map<std::wstring, std::unordered_map<unsigned int, ID3D11GeometryShader*	>> myGeometryShaders;
	std::unordered_map<unsigned int, ID3D11InputLayout*> myInputLayouts;

};
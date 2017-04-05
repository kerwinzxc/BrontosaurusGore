#pragma once
#include "BufferStructs.h"
#include "../CommonUtilities/StaticArray.h"

struct ID3D11VertexShader;
struct ID3D11PixelShader;
struct ID3D11GeometryShader;
struct ID3D11InputLayout;
struct ID3D11Buffer;
enum D3D_PRIMITIVE_TOPOLOGY;

class CDXFramework;


namespace CU
{
	class Camera;
}

class CEffect
{
public:
	CEffect(ID3D11VertexShader* aVertexShader, ID3D11PixelShader* aPixelShader,
		ID3D11GeometryShader* aGeometryShader, ID3D11InputLayout* aInputLayout,
		D3D_PRIMITIVE_TOPOLOGY aTopology, ID3D11VertexShader* aVertexInstancedShader = nullptr, ID3D11InputLayout* aInstancedLayout = nullptr, ID3D11PixelShader* aInstancedPixel = nullptr);

	CEffect(const CEffect& aEffect);
	CEffect(CEffect&& aTemporaryEffect) = delete;
	~CEffect();

	
	void Activate(const bool aInstanced = false);
	void ActivateForDepth(ID3D11PixelShader* aShadowShader = nullptr, const bool aInstanced = false);

private:
	CDXFramework* myFramework;

	ID3D11VertexShader* myVertexShader;
	ID3D11VertexShader* myVertexInstancedShader;

	ID3D11GeometryShader* myGeometryShader;

	ID3D11PixelShader* myPixelShader;
	ID3D11PixelShader* myPixelInstancedShader;



	ID3D11InputLayout* myLayout;
	ID3D11InputLayout* myInstancedLayout;
	
	D3D_PRIMITIVE_TOPOLOGY myTopology;
};

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
		D3D_PRIMITIVE_TOPOLOGY aTopology);

	CEffect(const CEffect& aEffect);
	CEffect(CEffect&& aTemporaryEffect) = delete;
	~CEffect();

	
	void Activate();
	void ActivateForDepth(ID3D11PixelShader* aShadowShader = nullptr);

private:
	CDXFramework* myFramework;

	ID3D11VertexShader* myVertexShader;
	ID3D11GeometryShader* myGeometryShader;
	ID3D11PixelShader* myPixelShader;

	ID3D11InputLayout* myLayout;
	
	D3D_PRIMITIVE_TOPOLOGY myTopology;
};

#include "stdafx.h"
#include "Effect.h"
#include "Engine.h"
#include "DXFramework.h"
#include "ConstBufferTemplate.h"
#include "DDSTextureLoader.h"
#include <Camera.h>
#include <d3d10.h>

CEffect::CEffect(ID3D11VertexShader* aVertexShader, ID3D11PixelShader* aPixelShader,
	ID3D11GeometryShader* aGeometryShader, ID3D11InputLayout* aInputLayout,
	D3D_PRIMITIVE_TOPOLOGY aTopology, ID3D11VertexShader* aVertexInstancedShader, ID3D11InputLayout* aInstancedLayout, ID3D11PixelShader* aPixelInstancedShader)
{
	myFramework = CEngine::GetInstance()->GetFramework();
	myVertexShader = aVertexShader;
	myPixelShader = aPixelShader;
	myGeometryShader = aGeometryShader;
	myLayout = aInputLayout;
	myTopology = aTopology;
	myPixelInstancedShader = aPixelInstancedShader;
	myVertexInstancedShader = aVertexInstancedShader;
	myInstancedLayout = aInstancedLayout;
}

CEffect::CEffect(const CEffect& aEffect)
{
	myFramework = aEffect.myFramework;

	myVertexShader = aEffect.myVertexShader;

	myGeometryShader = aEffect.myGeometryShader;

	myPixelShader = aEffect.myPixelShader;

	myLayout = aEffect.myLayout;

	myTopology = aEffect.myTopology;
	myPixelInstancedShader = aEffect.myPixelInstancedShader;
	myVertexInstancedShader = aEffect.myVertexInstancedShader;
	myInstancedLayout = aEffect.myInstancedLayout;
}




CEffect::~CEffect()
{
}

void CEffect::Activate(const bool aInstanced)
{
	assert((aInstanced && myVertexInstancedShader && myInstancedLayout) || (!aInstanced));
	if (aInstanced)
	{
		myFramework->GetDeviceContext()->VSSetShader(myVertexInstancedShader, NULL, 0);
		myFramework->GetDeviceContext()->IASetInputLayout(myInstancedLayout);
		myFramework->GetDeviceContext()->PSSetShader(myPixelInstancedShader, NULL, 0);
	}
	else
	{
		myFramework->GetDeviceContext()->VSSetShader(myVertexShader, NULL, 0);
		myFramework->GetDeviceContext()->IASetInputLayout(myLayout);
		myFramework->GetDeviceContext()->PSSetShader(myPixelShader, NULL, 0);
	}

	myFramework->GetDeviceContext()->GSSetShader(myGeometryShader, NULL, 0);
	myFramework->GetDeviceContext()->IASetPrimitiveTopology(myTopology);
}

void CEffect::ActivateForDepth(ID3D11PixelShader* aShadowShader, const bool aInstanced)
{
	assert((aInstanced && myVertexInstancedShader && myInstancedLayout) || (!aInstanced));
	if (aInstanced)
	{
		myFramework->GetDeviceContext()->VSSetShader(myVertexInstancedShader, NULL, 0);
		myFramework->GetDeviceContext()->IASetInputLayout(myInstancedLayout);
	}
	else
	{
		myFramework->GetDeviceContext()->VSSetShader(myVertexShader, NULL, 0);
		myFramework->GetDeviceContext()->IASetInputLayout(myLayout);
	}

	myFramework->GetDeviceContext()->PSSetShader(aShadowShader, NULL, 0);
	myFramework->GetDeviceContext()->GSSetShader(NULL, NULL, 0);
	myFramework->GetDeviceContext()->IASetPrimitiveTopology(myTopology);
}

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
	D3D_PRIMITIVE_TOPOLOGY aTopology)
{
	myFramework = CEngine::GetInstance()->GetFramework();
	myVertexShader = aVertexShader;
	//SAFE_ADD_REF(myVertexShader);
	myPixelShader = aPixelShader;
	//SAFE_ADD_REF(myPixelShader);
	myGeometryShader = aGeometryShader;
	//SAFE_ADD_REF(myGeometryShader);
	myLayout = aInputLayout;
	//SAFE_ADD_REF(myLayout);
	myTopology = aTopology;
	//SAFE_ADD_REF(myTopology);
}

CEffect::CEffect(const CEffect& aEffect)
{
	myFramework = aEffect.myFramework;

	//SAFE_RELEASE(myVertexShader);
	myVertexShader = aEffect.myVertexShader;
	//SAFE_ADD_REF(myVertexShader);

	//SAFE_RELEASE(myGeometryShader);
	myGeometryShader = aEffect.myGeometryShader;
	//SAFE_ADD_REF(myGeometryShader);

	//SAFE_RELEASE(myPixelShader);
	myPixelShader = aEffect.myPixelShader;
	//SAFE_ADD_REF(myPixelShader);

	//SAFE_RELEASE(myLayout);
	myLayout = aEffect.myLayout;
	//SAFE_ADD_REF(myLayout);

	//SAFE_RELEASE(myTopology);
	myTopology = aEffect.myTopology;
	//SAFE_ADD_REF(myTopology);

}




CEffect::~CEffect()
{
	//TELL SHADERMGR THAT WE DON'T WANT IT ANYMORE??


	//SAFE_RELEASE(myVertexShader);
	//SAFE_RELEASE(myGeometryShader);
	//SAFE_RELEASE(myPixelShader);
	//SAFE_RELEASE(myLayout);
}

void CEffect::Activate()
{
	myFramework->GetDeviceContext()->VSSetShader(myVertexShader, NULL, 0);
	myFramework->GetDeviceContext()->PSSetShader(myPixelShader, NULL, 0);
	myFramework->GetDeviceContext()->GSSetShader(myGeometryShader, NULL, 0);
	myFramework->GetDeviceContext()->IASetInputLayout(myLayout);
	myFramework->GetDeviceContext()->IASetPrimitiveTopology(myTopology);
}


void CEffect::ActivateForDepth(ID3D11PixelShader* aShadowShader)
{
	myFramework->GetDeviceContext()->VSSetShader(myVertexShader, NULL, 0);
	myFramework->GetDeviceContext()->PSSetShader(aShadowShader, NULL, 0);
	myFramework->GetDeviceContext()->GSSetShader(NULL, NULL, 0);
	myFramework->GetDeviceContext()->IASetInputLayout(myLayout);
	myFramework->GetDeviceContext()->IASetPrimitiveTopology(myTopology);
}


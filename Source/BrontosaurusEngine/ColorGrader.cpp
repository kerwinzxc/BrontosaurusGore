#include "stdafx.h"
#include "ColorGrader.h"
#include "ConstBufferTemplate.h"
#include "Engine.h"
#include "TextureManager.h"
#include "WindowsWindow.h"
#include "FullScreenHelper.h"
#include "ELUTType.h"

static_assert(ELUTType::eLength == 4, "hola espanjola");

CColorGrader::CColorGrader()
{
	myLerpTimeBuffer = nullptr;

	CU::Vector2ui windowSize = ENGINE->GetWindow()->GetWindowSize();
	myRenderPackage.Init(windowSize);

	CreateBuffer();
	LoadTextures();

	myFadeData.myFadeFrom = eDefault;
	myFadeData.myFadeTo = eDefault;
	myFadeData.myFadeTime = 0.0f;
}

CColorGrader::~CColorGrader()
{
	SAFE_RELEASE(myLerpTimeBuffer);
}

void CColorGrader::DoColorGrading(CRenderPackage& aRenderPackage, CFullScreenHelper& aFullscreenHelper, const float aDeltaTime)
{
	UpdateBuffer();

	myRenderPackage.Clear();
	myRenderPackage.Activate();
	
	ID3D11ShaderResourceView* SRV[3] =
	{
		aRenderPackage.GetResource(),
		myLuts[myFadeData.myFadeFrom]->GetShaderResourceView(),
		myLuts[myFadeData.myFadeTo]->GetShaderResourceView()
	};

	DEVICE_CONTEXT->PSSetShaderResources(1, 3, SRV);

	aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eColorGrading);

	SRV[0] = nullptr;
	SRV[1] = nullptr;
	SRV[2] = nullptr;
	DEVICE_CONTEXT->PSSetShaderResources(1, 3, SRV);

	aRenderPackage.Activate();
	aFullscreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, &myRenderPackage);
}

void CColorGrader::SetData(const ELUTType aFadeTo, const ELUTType aFadeFrom, const float aTime)
{
	myFadeData.myFadeTo = aFadeTo;
	myFadeData.myFadeFrom = aFadeFrom;
	myFadeData.myFadeTime = aTime;
}

void CColorGrader::LoadTextures()
{
	myLuts[eDefault] = &TEXTUREMGR.LoadTexture("Lut/defaultLUT.dds");
	myLuts[eHurt] = &TEXTUREMGR.LoadTexture("Lut/hurtLUT.dds");
	myLuts[eGUI] = &TEXTUREMGR.LoadTexture("Lut/pauseLUT.dds");
	myLuts[eNoChange] = &TEXTUREMGR.LoadTexture("Lut/NoChangeLUT.dds");
}

void CColorGrader::CreateBuffer()
{
	CU::Vector4f temp;
	myLerpTimeBuffer = BSR::CreateCBuffer<CU::Vector4f>(&temp);
}

void CColorGrader::UpdateBuffer()
{
	CU::Vector4f time;
	time.x = myFadeData.myFadeTime;
	BSR::UpdateCBuffer<CU::Vector4f>(myLerpTimeBuffer, &time);
	DEVICE_CONTEXT->PSSetConstantBuffers(2, 1, &myLerpTimeBuffer);
}


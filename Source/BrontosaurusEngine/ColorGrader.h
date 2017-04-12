#pragma once
#include "RenderPackage.h"
#include "SColorGradeFade.h"

enum ELUTType : char;

class CTexture;
struct ID3D11Buffer;

class CColorGrader
{
public:
	CColorGrader();
	~CColorGrader();
	
	void DoColorGrading(CRenderPackage& aRenderPackage, CFullScreenHelper& aFullscreenHelper, const float aDeltaTime);
	void SetData(const ELUTType aFadeTo, const ELUTType aFadeFrom, const float aTime);
private:
	void LoadTextures();
	void CreateBuffer();
	void UpdateBuffer();

private:
	CRenderPackage myRenderPackage;
	CU::StaticArray<CTexture*, 4> myLuts;
	ID3D11Buffer* myLerpTimeBuffer;
	SColorFade myFadeData;
};

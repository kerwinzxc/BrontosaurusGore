#include "stdafx.h"
#include "FT_FontFacade.h"
#include "FT_Font.h"
#include "../CommonUtilities/DL_Debug.h"


CFT_FontFacade::CFT_FontFacade()
{
	myFontpointer = nullptr;
}

CFT_FontFacade::CFT_FontFacade(CFT_Font* aFont)
{
	myFontpointer = aFont;
}

CFT_FontFacade::~CFT_FontFacade()
{
}

bool CFT_FontFacade::GetIfValid() const
{
	return myFontpointer != nullptr;
}

void CFT_FontFacade::SetSize(const int pt, const int aDeviceWidth, const unsigned aDeviceHeight) const
{
	if (myFontpointer != nullptr)
	{
		myFontpointer->SetSize(pt, aDeviceWidth, aDeviceHeight);
	}
}

ID3D11ShaderResourceView* CFT_FontFacade::GetCharResourceView(wchar_t aChar) const
{
	if (GetIfValid() != true)
	{
		DL_ASSERT("Font facade is not initilized properly");
	}
	return myFontpointer->GetCharResourceView(aChar);
}

CU::Vector2i CFT_FontFacade::GetAdvance(const wchar_t aNextChar, const wchar_t aPrevoiusChar, const bool aUseKerning) const
{
	return myFontpointer->GetAdvance(aNextChar, aPrevoiusChar, aUseKerning);
}

CEffect* CFT_FontFacade::GetEffect()
{
	return myFontpointer->GetEffect();
}

CU::Vector2i CFT_FontFacade::GetCharSize(const wchar_t aChar)
{
	return  myFontpointer->GetGlyphSize(aChar);
}

bool CFT_FontFacade::RequestCharSize(CU::Vector2i & aSizeOut, const wchar_t aChar)
{
	return myFontpointer->RequestGlyphSize(aSizeOut, aChar);
}

CU::Vector2i CFT_FontFacade::GetBearing(const wchar_t aChar)
{
	return  myFontpointer->GetBearing(aChar);
}

float CFT_FontFacade::GetlineHeight() const
{
	return  myFontpointer->GetlineHeight();
}

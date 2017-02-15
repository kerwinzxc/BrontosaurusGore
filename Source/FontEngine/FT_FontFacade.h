#pragma once
#include "../CommonUtilities/vector2.h"
#include <d3d11.h>
#include "../BrontosaurusEngine/Effect.h"

class CFT_Font;
class CFT_FontFacade
{
public:
	CFT_FontFacade();
	CFT_FontFacade(CFT_Font* aFont);
	~CFT_FontFacade();

	bool GetIfValid() const;

	void SetSize(const int pt, const int aDeviceWidth, const unsigned int aDeviceHeight) const;

	ID3D11ShaderResourceView* GetCharResourceView(wchar_t aChar) const;

	CU::Vector2i GetAdvance(const wchar_t aNextChar, const wchar_t aPrevoiusChar, const bool aUseKerning) const;
	CEffect* GetEffect();
	CU::Vector2i GetCharSize(const wchar_t aChar);
	bool RequestCharSize(CU::Vector2i& aSizeOut, const wchar_t aChar);
	CU::Vector2i GetBearing(const wchar_t aChar);
	float GetlineHeight() const;
private:
	CFT_Font* myFontpointer;
};


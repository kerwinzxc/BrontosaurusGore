#pragma once


#include "../CommonUtilities/vector2.h"
#include <unordered_map>
#include <d3d11.h>
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../BrontosaurusEngine/Effect.h"
#include <mutex>

class CTextBitmap;

namespace
{
	struct GlyphData
	{
		CU::Vector2i mySize;
		CU::Vector2i myBearing;
	};
}

class CFT_Font
{
public:
	CFT_Font();
	~CFT_Font();

	void SetSize(const int pt, const int aDeviceWidth, const unsigned int aDeviceHeight);
	void SetLineHeight(const int aLineHeight);

	ID3D11ShaderResourceView* GetCharResourceView(wchar_t aChar);

	CU::Vector2i GetAdvance(const wchar_t aNextChar, const wchar_t aPrevoiusChar, const bool aUseKerning);
	CU::Vector2i GetAdvance(const FT_UInt aNextGlyph, const FT_UInt aPrevoiusGlyph, const bool aUseKerning);

	CU::Vector2i GetGlyphSize(wchar_t aChar);
	bool RequestGlyphSize(CU::Vector2i& aSizeOut, wchar_t aChar);

	CU::Vector2i GetBearing(const wchar_t aChar);
	CU::Vector2i GetBearing(const FT_UInt aChar);

	GlyphData GetGlyphData(unsigned);

	//void SetLineGap(const int pt)

	CEffect* GetEffect() const;
	float GetlineHeight() const;
	FT_Face myFace;
	const char* myFacePath;

private:

	FT_Error LoadGlyph(FT_UInt aGlyphIndex);

	CTextBitmap RenderChar(wchar_t aChar);
	CTextBitmap RenderChar(unsigned  aGlyphIndex);
	void CreateCharTexture(const FT_UInt aGlyphIndex);
	void CreateEffect();
	std::unordered_map<unsigned, ID3D11ShaderResourceView*> myRenderedGlyphs;
	std::unordered_map<unsigned, GlyphData> myGlyphData;

	int myLineHeight;

	CEffect* myEffect;

	struct
	{
		FT_UInt myGlyphIndex;
		FT_GlyphSlot mySlot;
	} myCurrentGlyph;

};


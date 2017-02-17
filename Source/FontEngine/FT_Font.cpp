#include "stdafx.h"
#include "FT_Font.h"
#include "../CommonUtilities/CommonUtilities.h"
#include "../BrontosaurusEngine/TextBitmap.h"
#include "../CommonUtilities/DL_Debug.h"
#include "../CommonUtilities/vector2.h"
#include "../BrontosaurusEngine/Engine.h"
#include "../BrontosaurusEngine/DXFramework.h"

#include FT_FREETYPE_H
#include "../BrontosaurusEngine/EModelBluePrint.h"
#include "../BrontosaurusEngine/Effect.h"
#include "../BrontosaurusEngine/ShaderManager.h"

#ifndef ERROR_CHECK
#define ERROR_CHECK(aError, msg) assert(aError == FT_Err_Ok && msg);
#endif

#define SAFE_RELEASE(comptr) if (comptr != nullptr) { comptr->Release(); comptr = nullptr; }

//#define LETTER_STRING L"ABCDEFGHIJKLMNOPQRSTUVXYZabcdefghijklmnopqrstuvxyz.,!:; "

CFT_Font::CFT_Font()
{
	myLineHeight = 0;
	myFace = nullptr;
	myFacePath = nullptr;

	myCurrentGlyph.myGlyphIndex = 0;
	myCurrentGlyph.mySlot = nullptr;
	myEffect = nullptr;

	CreateEffect();
}


CFT_Font::~CFT_Font()
{
	SAFE_DELETE(myEffect);

	for (auto it = myRenderedGlyphs.begin(); it != myRenderedGlyphs.end(); ++it)
	{
		SAFE_RELEASE(it->second);
	}
}

void CFT_Font::SetSize(const int pt, const  int aDeviceWidth, const unsigned int aDeviceHeight)
{
	FT_Error error;

	/*bool isScalable = FT_IS_SCALABLE(myFace);
	error = FT_Set_Char_Size(myFace, 0, pt , 0, 0);
	ERROR_CHECK(error, "failed to set Face char size");*/

	error = FT_Set_Pixel_Sizes(myFace, pt, pt);
	ERROR_CHECK(error, "failed to set Face pixel sizes");
}

void CFT_Font::SetLineHeight(const int aLineHeight)
{
	myLineHeight = aLineHeight;
}

ID3D11ShaderResourceView* CFT_Font::GetCharResourceView(wchar_t aChar)
{
	const FT_UInt glyphIndex = FT_Get_Char_Index(myFace, aChar);
	if (myRenderedGlyphs.count(glyphIndex) < 1)
	{
		CreateCharTexture(glyphIndex);
	}
	ID3D11ShaderResourceView* temp;
	temp = myRenderedGlyphs[glyphIndex];
	return temp;
}

CU::Vector2i CFT_Font::GetAdvance(const wchar_t aNextChar, const wchar_t aPrevoiusChar, const bool aUseKerning)
{
	const FT_UInt nextGlyph = FT_Get_Char_Index(myFace, aNextChar);
	const FT_UInt previousGlyph = FT_Get_Char_Index(myFace, aPrevoiusChar);
	return GetAdvance(nextGlyph, previousGlyph, aUseKerning);
}

CU::Vector2i CFT_Font::GetAdvance(const FT_UInt aNextGlyph, const FT_UInt aPrevoiusGlyph, const bool aUseKerning)
{
	FT_Vector ftAdvance;

	const FT_UInt resetGlyph = myCurrentGlyph.myGlyphIndex;
	LoadGlyph(aPrevoiusGlyph);
	
	ftAdvance = myCurrentGlyph.mySlot->advance;
	
	LoadGlyph(resetGlyph);

	if (aUseKerning == true)
	{
		FT_Vector delta;

		FT_Get_Kerning(myFace, aPrevoiusGlyph, aNextGlyph, FT_KERNING_DEFAULT, &delta);

		ftAdvance.x += delta.x;
	}

	const CU::Vector2i finalAdvance(ftAdvance.x / 64, ftAdvance.y / 64);

	return finalAdvance;
}

CU::Vector2i CFT_Font::GetGlyphSize(wchar_t aChar)
{
	const FT_UInt glyphIndex = FT_Get_Char_Index(myFace, aChar);
	if (myRenderedGlyphs.count(glyphIndex) < 1)
	{
		CreateCharTexture(glyphIndex);
	}
	return myGlyphData[glyphIndex].mySize;
}

bool CFT_Font::RequestGlyphSize(CU::Vector2i & aSizeOut, wchar_t aChar)
{
	const FT_UInt glyphIndex = FT_Get_Char_Index(myFace, aChar);
	if (myRenderedGlyphs.count(glyphIndex) < 1)
	{
		return false;
	}

	aSizeOut = myGlyphData[glyphIndex].mySize;
	
	return true;
}

CU::Vector2i CFT_Font::GetBearing(wchar_t aChar)
{
	const FT_UInt glyphIndex = FT_Get_Char_Index(myFace, aChar);
	return GetBearing(glyphIndex);
}

CU::Vector2i CFT_Font::GetBearing(const FT_UInt aChar)
{
	if (myGlyphData.count(aChar) < 1)
	{
		CreateCharTexture(aChar);
	}

	return myGlyphData[aChar].myBearing;
}

CEffect* CFT_Font::GetEffect() const
{
	return myEffect;
}

float CFT_Font::GetlineHeight() const
{
	//return myFace->height / 64;
	return static_cast<float>(myLineHeight);
}

FT_Error CFT_Font::LoadGlyph(FT_UInt aGlyphIndex)
{
	if (myCurrentGlyph.mySlot == nullptr || aGlyphIndex != myCurrentGlyph.myGlyphIndex)
	{
		const FT_Error error = FT_Load_Glyph(myFace, aGlyphIndex, FT_LOAD_DEFAULT);
		myCurrentGlyph.mySlot = myFace->glyph;
		return error;
	}

	return static_cast<FT_Error>(0);
}

CTextBitmap CFT_Font::RenderChar(wchar_t aChar)
{
	const FT_UInt glyphIndex= FT_Get_Char_Index(myFace, aChar);
	return  RenderChar(glyphIndex);
}

CTextBitmap CFT_Font::RenderChar(FT_UInt aGlyphIndex) 
{
	FT_Error error;

	error = LoadGlyph(aGlyphIndex);

	if (error != 0)
	{
		CU::DynamicString bla("Freetype load glyph failed check on \"https://www.freetype.org/freetype2/docs/reference/ft2-error_code_values.html#FT_Err_XXX\" code: ");
		bla += error;
		DL_ASSERT(bla.c_str());
	}

	error = FT_Render_Glyph(myFace->glyph, FT_RENDER_MODE_NORMAL);
	if (error != 0)
	{
		CU::DynamicString bla("Freetype render glyph failed check on \"https://www.freetype.org/freetype2/docs/reference/ft2-error_code_values.html#FT_Err_XXX\" code: ");
		bla += error;
		DL_ASSERT(bla.c_str());
	}
	FT_GlyphSlot const glyphSlot = myFace->glyph;

	CTextBitmap charBitmap;
	if (glyphSlot->bitmap.width * glyphSlot->bitmap.rows > 0)
	{
		charBitmap.Init(glyphSlot->bitmap.width, glyphSlot->bitmap.rows);
		charBitmap.DrawMono(CU::Vector2i(0, 0), { static_cast<int>(glyphSlot->bitmap.width), static_cast<int>(glyphSlot->bitmap.rows) }, glyphSlot->bitmap.buffer, CU::Vector3uc(0xff, 0xff, 0xff));
	}
	else
	{
		charBitmap.Init(1, 1);
	}
	myGlyphData[aGlyphIndex].myBearing = CU::Vector2i(glyphSlot->bitmap_left, glyphSlot->bitmap_top);
	return charBitmap;
}

void CFT_Font::CreateCharTexture(const FT_UInt aGlyphIndex)
{
	myGlyphData[aGlyphIndex] = GlyphData();

	CTextBitmap bitmap = RenderChar(aGlyphIndex);

	D3D11_SUBRESOURCE_DATA subResourceData;
	ZeroMemory(&subResourceData, sizeof(subResourceData));

	subResourceData.pSysMem = bitmap.CopyBitmapData();
	subResourceData.SysMemPitch = bitmap.GetMemPitch();
	subResourceData.SysMemSlicePitch = bitmap.GetByteSize();

	D3D11_TEXTURE2D_DESC texture2DDesc;

	texture2DDesc.Width = bitmap.GetWidth();
	texture2DDesc.Height = bitmap.GetHeight();
	texture2DDesc.MipLevels = 1;
	texture2DDesc.ArraySize = 1;
	texture2DDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	texture2DDesc.SampleDesc.Count = 1;
	texture2DDesc.SampleDesc.Quality = 0;
	texture2DDesc.Usage = D3D11_USAGE_DYNAMIC;
	texture2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	texture2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	texture2DDesc.MiscFlags = 0;

	ID3D11Texture2D * texture2D = nullptr;

	HRESULT result = S_OK;
	result = CEngine::GetInstance()->GetFramework()->GetDevice()->CreateTexture2D(&texture2DDesc, &subResourceData, &texture2D);
	if (result != S_OK)
	{
		DL_ASSERT("Failed creating text texture");
	}

	ID3D11ShaderResourceView* resourceView = nullptr;

	result = CEngine::GetInstance()->GetFramework()->GetDevice()->CreateShaderResourceView(texture2D, NULL, &resourceView);
	if (result != S_OK)
	{
		DL_ASSERT("Failed creating text shader resource");
	}

	myRenderedGlyphs[aGlyphIndex] = resourceView;
	myGlyphData[aGlyphIndex].mySize = CU::Vector2i(bitmap.GetWidth(), bitmap.GetHeight());
	
	texture2D->Release();
}

void CFT_Font::CreateEffect()
{
	const unsigned shaderBlueprint = EModelBluePrint_Sprite2D;
	ID3D11PixelShader* pixeruShaderu = SHADERMGR->LoadPixelShader(L"Shaders/sprite_shader.fx", shaderBlueprint);
	ID3D11VertexShader* vertexShader = SHADERMGR->LoadVertexShader(L"Shaders/sprite_shader.fx", shaderBlueprint);
	ID3D11InputLayout* inputLayout = SHADERMGR->LoadInputLayout(L"Shaders/sprite_shader.fx", shaderBlueprint);

	myEffect = new CEffect(vertexShader, pixeruShaderu, nullptr, inputLayout, D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

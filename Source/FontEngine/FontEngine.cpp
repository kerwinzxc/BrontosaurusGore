#include "stdafx.h"
#include "FontEngine.h"
#include "FT_Font.h"
#include "../CommonUtilities/JsonValue.h"
#include "../CommonUtilities/DL_Debug.h"


#ifndef ERROR_CHECK
#define ERROR_CHECK(aError, msg) assert(aError == FT_Err_Ok && msg);
#endif

CFontEngine* CFontEngine::ourInstance = nullptr;


CFontEngine::CFontEngine()
{
	myFreetypeAPI = nullptr;
}


CFontEngine::~CFontEngine()
{
	for (auto i = myFaces.begin(); i != myFaces.end(); ++i)
	{
		delete i->second;
		i->second = nullptr;
	}
}

void CFontEngine::Init()
{
	FT_Error error;
	error = FT_Init_FreeType(&myFreetypeAPI);
	ERROR_CHECK(error, "failed to create FREETYPE2 API");

	LoadFonts();
}

CFT_FontFacade CFontEngine::GetFace(const char* aFontName)
{
	if (myFaces.count(aFontName) < 1)
	{
		std::string errorString = "No font loaded with the name ";
		errorString += aFontName;
		DL_ASSERT(errorString.c_str());
	}

	return CFT_FontFacade(myFaces[aFontName]);
}

void CFontEngine::LoadFonts(std::string aFontJsonFile)
{
	CU::CJsonValue fontJsonFile;
	const std::string error = fontJsonFile.Parse(aFontJsonFile);
	if (error != "")
	{
		DL_ASSERT(error.c_str());
		return;
	}

	const CU::CJsonValue fontsArray = fontJsonFile.at("fonts");
	for (int i = 0; i < fontsArray.Size(); ++i)
	{
		const CU::CJsonValue fontValue = fontsArray[i];

		const std::string fontName = fontValue.at("name").GetString();
		const std::string fontFile = fontValue.at("file").GetString();
		const int fontSize = fontValue.at("size").GetInt();
		const int lineHeight = fontValue.at("lineHeight").GetInt();

		CFT_Font* font = CreateFace(fontFile.c_str());
		font->SetSize(fontSize, 0, 0);
		font->SetLineHeight(lineHeight);

		myFaces[fontName] = font;
	}
}

CFT_Font* CFontEngine::CreateFace(const char* aFilePath)
{
	CFT_Font* font = new CFT_Font();
	font->myFacePath = aFilePath;

	FT_Error error = 0;
	error = FT_New_Face(myFreetypeAPI, aFilePath, 0, &font->myFace);

	ERROR_CHECK(error, "failed to create FREETYPE2 Face from file");
	return font;
}

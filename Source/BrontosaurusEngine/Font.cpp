#include "stdafx.h"
#include "Font.h"

#include <fstream>
#include <string>
#include "Texture.h"
#include "Engine.h"
#include "TextureManager.h"
#include "ShaderManager.h"
#include "Effect.h"
#include "EModelBluePrint.h"

#include <d3dcommon.h>

CFont::CFont(const CU::DynamicString& aFontPath)
	: myCharacters(CHAR_MAX)
	, myCharacterAtlas(nullptr)
	, myEffect(nullptr)
	, myLineHeight(0.f)
	, myRefCount(0)
{
	LoadFont(aFontPath);
	CreateEffect();
}

CFont::~CFont()
{
	CEngine::GetInstance()->GetTextureManager().DestroyTexture(myCharacterAtlas);
	SAFE_DELETE(myEffect);
}

void CFont::LoadFont(const CU::DynamicString& aFontPath)
{
	std::ifstream fontFile(aFontPath.c_str());
	if (fontFile.good() == false)
	{
		DL_ASSERT("Could not load font file: %s\n", aFontPath.c_str());
		return;
	}

	size_t position = 0;
	std::string line = "";

	while ((position = line.find("lineHeight=")) == std::string::npos)
	{
		std::getline(fontFile, line);
	}

	std::string lineHeightStr = line.substr(position + std::strlen("lineHeight="), 3);
	myLineHeight = static_cast<float>(std::stoi(lineHeightStr));

	while ((position = line.find("file=")) == std::string::npos)
	{
		std::getline(fontFile, line);
	}

	std::string texturePath = line.substr(position + std::strlen("file=\""));
	texturePath.pop_back();

	texturePath = aFontPath.SubStr(0, aFontPath.Size() - 4).c_str();
	texturePath += ".dds";
	LoadTexture(texturePath.c_str());

	while (line.find("chars") == std::string::npos)
	{
		std::getline(fontFile, line);
	}

	int characterCount = std::stoi(line.substr(std::strlen("chars count=")));

	myCharacters.Resize(CHAR_MAX);

	for (char i = CHAR_MAX - static_cast<char>(characterCount); i < CHAR_MAX; ++i)
	{
		std::getline(fontFile, line);

		size_t index = 0;
		std::string numberString = "";

		index = line.find("id=");
		numberString = line.substr(index + std::strlen("id="), 3);
		myCharacters[i].id = std::stoi(numberString);

		index = line.find("x=");
		numberString = line.substr(index + std::strlen("x="), 3);
		myCharacters[i].x = std::stoi(numberString);

		index = line.find("y=");
		numberString = line.substr(index + std::strlen("y="), 3);
		myCharacters[i].y = std::stoi(numberString);

		index = line.find("width=");
		numberString = line.substr(index + std::strlen("width="), 2);
		myCharacters[i].width = std::stoi(numberString);

		index = line.find("height=");
		numberString = line.substr(index + std::strlen("height="), 2);
		myCharacters[i].height = std::stoi(numberString);

		index = line.find("xoffset=");
		numberString = line.substr(index + std::strlen("xoffset="), 2);
		myCharacters[i].xOffset = std::stoi(numberString);

		index = line.find("yoffset=");
		numberString = line.substr(index + std::strlen("yoffset="), 2);
		myCharacters[i].yOffset = std::stoi(numberString);

		index = line.find("xadvance=");
		numberString = line.substr(index + std::strlen("xadvance="), 2);
		myCharacters[i].xAdvance = std::stoi(numberString);
	}
}

inline void CFont::LoadTexture(const char* aFontPath)
{
	myCharacterAtlas = &CEngine::GetInstance()->GetTextureManager().LoadTexture(aFontPath);
}

void CFont::CreateEffect()
{
	unsigned int shaderType = EModelBluePrint_Text2D;
	ID3D11VertexShader* vertexShader = CEngine::GetInstance()->GetShaderManager()->LoadVertexShader(L"Shaders/text_shader.fx", shaderType);
	ID3D11PixelShader* pixelShader = CEngine::GetInstance()->GetShaderManager()->LoadPixelShader(L"Shaders/text_shader.fx", shaderType);
	ID3D11InputLayout* inputLayout = CEngine::GetInstance()->GetShaderManager()->LoadInputLayout(L"Shaders/text_shader.fx", shaderType);

	myEffect = new CEffect(vertexShader, pixelShader, nullptr, inputLayout, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

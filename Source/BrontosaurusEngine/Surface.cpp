#include "stdafx.h"
#include "Surface.h"
#include "Engine.h"
#include "DXFramework.h"
#include "TextureManager.h"
#include <StaticArray.h>
#include "StringHelper.h"
#include <D3D11.h>

#define NUM_TEXTURES 16


CSurface::CSurface(const std::string& aDirectory, const CU::GrowingArray<std::string>& aTextureNameList)
	: myFramework(nullptr)
	, myTextures(8)
{
	myFramework = CEngine::GetInstance()->GetFramework();

	for (unsigned int i = 0; i < aTextureNameList.Size(); ++i)
	{
		if (i != (eDiffuse)				&&
			i != (eRoughness)			&&
			i != (eAmbientOcclusion)	&&
			i != (eEmissive)			&&
			i != (eNormal)				&&
			i != (eNormal2)				&&
			i != (eMetalness))
		{
			continue;

		}

		{
			if (i == eEmissive && aTextureNameList[i] == "")
			{
				myTextures.Add(nullptr);
				continue;
			}
			std::wstring texturePath = CU::StringToWString(aDirectory + aTextureNameList[i]);
			CTexture& texture = CEngine::GetInstance()->GetTextureManager().LoadTexture(texturePath.c_str());
			myTextures.Add(&texture);
		}
	}
}

CSurface::CSurface(const CU::GrowingArray<const wchar_t*>& aTexturePathList)
	: myFramework(nullptr)
	, myTextures(8)
{
	myFramework = CEngine::GetInstance()->GetFramework();

	for (unsigned int i = 0; i < aTexturePathList.Size(); ++i)
	{
		if (aTexturePathList[i][0] == '\0')
		{
			myTextures.Add(nullptr);
		}
		else
		{
			CTexture& texture = CEngine::GetInstance()->GetTextureManager().LoadTexture(aTexturePathList[i]);
			myTextures.Add(&texture);
		}
	}
}

CSurface::CSurface(const CSurface& aSurface)
	: myTextures(aSurface.myTextures)
	, myFramework(aSurface.myFramework)
{
	for (CTexture* texture : myTextures)
	{
		if (texture)
		{
			texture->AddRef();
		}
	}
}

CSurface::~CSurface()
{
	for (unsigned int i = 0; i < myTextures.Size(); ++i)
	{
		CEngine::GetInstance()->GetTextureManager().DestroyTexture(myTextures[i]);
	}
}

CSurface& CSurface::operator=(const CSurface& aSurface)
{
	myTextures = aSurface.myTextures;
	myFramework = aSurface.myFramework;

	for (CTexture* texture : myTextures)
	{
		if (texture)
		{
			texture->AddRef();
		}
	}

	return *this;
}

void CSurface::Activate()
{
	ID3D11ShaderResourceView* shaderResourceViewPP[NUM_TEXTURES]{NULL};

	for (unsigned int i = 0; i < myTextures.Size() && i < NUM_TEXTURES; ++i)
	{
		if (myTextures[i] != nullptr)
		{
			shaderResourceViewPP[i] = myTextures[i]->GetShaderResourceView();
		}
	}

	//slot 1 because cubemap ocupies (?) slot 0
	myFramework->GetDeviceContext()->PSSetShaderResources(1, min(myTextures.Size(), NUM_TEXTURES), shaderResourceViewPP);
	myFramework->GetDeviceContext()->VSSetShaderResources(1, min(myTextures.Size(), NUM_TEXTURES), shaderResourceViewPP); // Edvin Fuckar till lite

}

const CU::Vector2ui& CSurface::GetTextureSize() const
{
	assert(myTextures.Size() > 0 && "should not get texture size from surface of no textures");
	if (myTextures.Empty() || myTextures.GetFirst() == nullptr)
	{
		return CU::Vector2ui::Zero;
	}

	return myTextures[0]->GetSize();
}

CU::Vector2f CSurface::GetTextureSizeFloat() const
{
	if (myTextures.Size() > 0 && myTextures.GetFirst() != nullptr)
	{
		return CU::Vector2f(myTextures.GetFirst()->GetSize());
	}

	return CU::Vector2f::Zero;
}

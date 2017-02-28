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



		if (aTextureNameList[i] == "")
		{
			myTextures.Add(nullptr);
		}
		else
		{
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

CSurface::CSurface(const CSurface & aSurface)
{
	myFramework = aSurface.myFramework;
	for (unsigned int i = 0; i < aSurface.myTextures.Size(); ++i)
	{
		myTextures.Add(aSurface.myTextures[i]);
		myTextures.GetLast()->AddRef();
	}
}

CSurface::~CSurface()
{
	//for (unsigned int i = 0; i < myShaderResources.Size(); ++i)
	//{
	//	SAFE_RELEASE(myShaderResources[i]);
	//}

	for (unsigned int i = 0; i < myTextures.Size(); ++i)
	{
		CEngine::GetInstance()->GetTextureManager().DestroyTexture(myTextures[i]);
	}
}

void CSurface::Activate()
{
	ID3D11ShaderResourceView* shaderResourceViewPP[NUM_TEXTURES]{NULL};

	for (unsigned int i = 0; i < myTextures.Size() && i < NUM_TEXTURES; ++i)
	{
		if (myTextures[i] != nullptr)
		{
			shaderResourceViewPP[i] = *myTextures[i]->GetShaderResourceViewPointer();
		}
	}

	//slot 1 because cubemap ocupies (?) slot 0
	myFramework->GetDeviceContext()->PSSetShaderResources(1, min(myTextures.Size(), NUM_TEXTURES), shaderResourceViewPP);
	myFramework->GetDeviceContext()->VSSetShaderResources(1, min(myTextures.Size(), NUM_TEXTURES), shaderResourceViewPP); // Edvin Fuckar till lite

}

const CU::Vector2ui& CSurface::GetTextureSize() const
{
	assert(myTextures.Size() > 0 && "should not get texture size from surface of no textures");
	return myTextures[0]->GetSize();
}

CU::Vector2f CSurface::GetTextureSizeFloat() const
{
	if (myTextures.Size() > 0)
	{
		return CU::Vector2f(myTextures[0]->GetSize());
	}

	return CU::Vector2f::Zero;
}

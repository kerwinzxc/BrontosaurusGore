#include "stdafx.h"
#include "Texture.h"

#include <d3d11.h>

CTexture::CTexture()
{
	myTexture = nullptr;
	myRefCount = 0;
}

CTexture::~CTexture()
{
	SAFE_RELEASE(myTexture);
}

CTexture& CTexture::operator=(const CTexture & aTexture)
{
	SAFE_RELEASE(myTexture);
	myTexture = aTexture.myTexture;
	
	if (myTexture != nullptr)
	{
		myTexture->AddRef();
	}

	return *this;
}

CTexture& CTexture::operator=(CTexture&& aTexture)
{
	SAFE_RELEASE(myTexture);
	myTexture = aTexture.myTexture;
	aTexture.myTexture = nullptr;

	return *this;
}


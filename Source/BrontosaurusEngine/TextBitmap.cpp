#include "stdafx.h"
#include "TextBitmap.h"
#include <d3d11.h>


CTextBitmap::CTextBitmap(): myWidth(0), myHeigth(0), myBitmap(nullptr), myUpToDate(false)
{
}

CTextBitmap::CTextBitmap(const CTextBitmap& aBitmap) : CTextBitmap()
{
	myHeigth = aBitmap.myHeigth;
	myWidth = aBitmap.myWidth;

	if (myBitmap != nullptr)
	{
		delete[] myBitmap;
		myBitmap = nullptr;
	}

	myBitmap = aBitmap.CopyBitmapData();
}

CTextBitmap::~CTextBitmap()
{
	delete[] myBitmap;
	myBitmap = nullptr;
	
}

void CTextBitmap::Init(int aWidth, int aHeight)
{
	if (myBitmap != nullptr)
	{
		delete[] myBitmap;
		myBitmap = nullptr;
	}

	myBitmap = new unsigned char[aWidth * aHeight * 4];
	myWidth = aWidth;
	myHeigth = aHeight;

	SecureZeroMemory(myBitmap, GetByteSize());
}

void CTextBitmap::DrawMono(const CU::Vector2i& aPosition, const CU::Vector2i& aSize, unsigned char* aBuffer, const CU::Vector3<unsigned char> aColour)
{
	CU::Vector2i penPosition(0, 0);

	if (aSize.x > myWidth || aSize.y > myHeigth)
	{
		DL_ASSERT("source is larger thatn target");
	}

	for (int i = 0; i < aSize.x * aSize.y; ++i)
	{
		const CU::Point2i currentPosition = aPosition + penPosition;
		const int pixelIndex = (myWidth * currentPosition.y + currentPosition.x) * 4;

		if (aBuffer[aSize.x * penPosition.y + penPosition.x] != 0)
		{
			const int index_1 = pixelIndex;
			myBitmap[index_1] = aColour.r;

			const int greenIndex = pixelIndex + 1;
			myBitmap[greenIndex] = aColour.g;

			const int blueIndex = pixelIndex + 2;
			myBitmap[blueIndex] = aColour.b;
		}
		const int alphaIndex = pixelIndex + 3;
		if (alphaIndex > myHeigth * myWidth * 4 + 3)
		{
			DL_ASSERT("writing outside data");
		}

		myBitmap[alphaIndex] = aBuffer[aSize.x * penPosition.y + penPosition.x];

		penPosition.x += 1;
		if (penPosition.x >= aSize.x)
		{
			penPosition.x = 0;
			penPosition.y += 1;
		}
	}
	myUpToDate = false;
}

unsigned char* CTextBitmap::CopyBitmapData() const
{
	if (myBitmap != nullptr )
	{
		unsigned char* copy = new unsigned char[myHeigth * myWidth * 4];

		memcpy_s(copy, GetByteSize(),  myBitmap, GetByteSize());

		/*for (int i = 0; i < myWidth * myHeigth * 4; ++i)
		{
			copy[i] = myBitmap[i];
		}*/

		return copy;
	}

	return nullptr;
}

CTextBitmap CTextBitmap::operator=(const CTextBitmap& aSecondBitmap)
{
	myHeigth = aSecondBitmap.myHeigth;
	myWidth = aSecondBitmap.myWidth;

	if (myBitmap != nullptr)
	{
		delete[] myBitmap;
		myBitmap = nullptr;
	}

	myBitmap = aSecondBitmap.CopyBitmapData();

	return *this;
}

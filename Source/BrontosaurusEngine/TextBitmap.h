#pragma once

#include "../CommonUtilities/vector2.h"
#include "../CommonUtilities/vector3.h"

enum class eBitmapFormat
{
	MONO,
	RGB
};

class CTextBitmap
{
public:
	CTextBitmap();
	CTextBitmap(const CTextBitmap& aBitmap);
	~CTextBitmap();

	void Init(int aWidth, int aHeight);

	void DrawMono(const CU::Vector2i &aPosition, const CU::Vector2i &aSize, unsigned char* aBuffer, const CU::Vector3<unsigned char> aColour = CU::Vector3<unsigned char>(0,0,0));
	unsigned char* CopyBitmapData() const;
	unsigned int GetByteSize() const;
	unsigned int GetMemPitch() const;

	int GetWidth()const;
	int GetHeight()const;

	CTextBitmap operator=(const CTextBitmap &);
private:
	int myWidth;
	int myHeigth;

	unsigned char* myBitmap;

	bool myUpToDate;
};

inline unsigned CTextBitmap::GetByteSize() const
{
	return sizeof(unsigned char) * myWidth * myHeigth * 4;
}

inline unsigned CTextBitmap::GetMemPitch() const
{
	return sizeof(unsigned char) * myWidth * 4;
}

inline int CTextBitmap::GetWidth() const
{
	return myWidth;
}

inline int CTextBitmap::GetHeight() const
{
	return myHeigth;
}

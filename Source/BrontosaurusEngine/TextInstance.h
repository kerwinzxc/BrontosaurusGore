#pragma once
#include "../CommonUtilities/DynamicString.h"
#include "../CommonUtilities/vector4.h"
#include "../CommonUtilities/vector2.h"
#include "Text.h"
#include "Allignment.h"

class CText;



class CTextInstance
{
public:
	static const CU::Vector4f Red;
	static const CU::Vector4f Yellow;
	static const CU::Vector4f Green;
	static const CU::Vector4f Blue;
	static const CU::Vector4f Black;
	static const CU::Vector4f White;

	CTextInstance();
	CTextInstance(const CTextInstance& aTextInstance);
	~CTextInstance();

	void Init(const CU::DynamicString& aFontPath = "Default");
	void Render() const;

	void SetLineGap();

	inline void SetPosition(const CU::Vector2f& aPosition);
	inline const CU::Vector2f& GetPosition() const;

	inline void SetColor(const CU::Vector4f& aColor);
	inline const CU::Vector4f& GetColor() const;

	bool SetTextLine(const unsigned int aLineNumber, const CU::DynamicString& aString);
	bool SetTextLine(const unsigned int aLineNumber, CU::DynamicString&& aString);
	void SetTextLines(const CU::GrowingArray<CU::DynamicString>& someLines);
	void SetTextLines(CU::GrowingArray<CU::DynamicString>&& someLines);
	const CU::GrowingArray<CU::DynamicString> &GetTextLines();

	inline void SetText(const CU::DynamicString& aString);
	inline CU::DynamicString GetText() const;
	float GetlineHeight() const;

	CU::Vector2f GetQuadSizeNormalized() const;

	void SetAlignment(eAlignment anAlignment);

	CTextInstance& operator=(const CTextInstance& aTextInstance);
private:
	//CU::DynamicString myString;
	CU::GrowingArray<CU::DynamicString> myStrings;
	CU::Vector4f myColor;
	CU::Vector2f myPosition;
	CText* myText;
	CU::Vector2f myLineGap;

	eAlignment myAlignemnt;
};

inline void CTextInstance::SetPosition(const CU::Vector2f & aPosition)
{
	myPosition = aPosition;
}

inline const CU::Vector2f & CTextInstance::GetPosition() const
{
	return myPosition;
}

inline void CTextInstance::SetColor(const CU::Vector4f& aColor)
{
	myColor = aColor;
}

inline const CU::Vector4f& CTextInstance::GetColor() const
{
	return myColor;
}

inline void CTextInstance::SetText(const CU::DynamicString& aString)
{

	myStrings.RemoveAll();
	int lastPosition = 0;
	for (int i = 0; i < aString.Size(); ++i)
	{
		if (aString.at(i) == '\n')
		{
			DL_PRINT_WARNING("Warning slow! consider using different set text method");
			const CU::DynamicString explainingString(aString.SubStr(lastPosition, i - lastPosition));
			myStrings.Add(explainingString);
			lastPosition = i + 1;
		}
	}

	if (lastPosition == 0)
	{
		myStrings.Add(aString);
	}
	else
	{
		const  CU::DynamicString lastExplaingString(aString.SubStr(lastPosition, aString.Size() - lastPosition));
		myStrings.Add(lastExplaingString);
	}
}

inline CU::DynamicString CTextInstance::GetText() const
{

	CU::DynamicString string;
	for (unsigned int i = 0; i < myStrings.Size(); ++i)
	{
		if (i != 0)
		{
			DL_PRINT_WARNING("Warning slow! please consider just getting a separete line");
			string += '/n';
		}
		string += myStrings[i];
	}

	return string;
}

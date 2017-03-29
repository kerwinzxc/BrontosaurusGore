#pragma once
#include "../CommonUtilities/vector4.h"
#include "../CommonUtilities/vector2.h"
#include "Text.h"
#include "Allignment.h"
#include <string>
#include "../CommonUtilities/GrowingArray.h"
#include "../CommonUtilities/DL_Debug.h"

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
	void Init(const std::string& aFontPath = "Default");

	void Render() const;
	void RenderToGUI(std::wstring anElementName) const;

	inline void SetPosition(const CU::Vector2f& aPosition);
	inline const CU::Vector2f& GetPosition() const;

	inline void SetColor(const CU::Vector4f& aColor);
	inline const CU::Vector4f& GetColor() const;

	bool SetTextLine(const unsigned int aLineNumber, const std::wstring& aString);
	bool SetTextLine(const unsigned int aLineNumber, std::wstring&& aString);
	void SetTextLines(const CU::GrowingArray<std::wstring>& someLines);
	void SetTextLines(CU::GrowingArray<std::wstring>&& someLines);
	const CU::GrowingArray<std::wstring> &GetTextLines();

	inline void SetText(const std::wstring& aString);
	inline std::wstring GetText() const;
	float GetlineHeight() const;

	CU::Vector2f GetQuadSizeNormalized() const;

	void SetAlignment(eAlignment anAlignment);

	CTextInstance& operator=(const CTextInstance& aTextInstance);

private:
	CU::GrowingArray<std::wstring> myStrings;
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

inline void CTextInstance::SetText(const std::wstring& aString)
{

	myStrings.RemoveAll();
	int lastPosition = 0;
	for (int i = 0; i < aString.size(); ++i)
	{
		if (aString.at(i) == '\n')
		{
			DL_PRINT_WARNING("Warning slow! consider using different set text method");
			const std::wstring explainingString(aString.substr(lastPosition, i - lastPosition));
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
		const  std::wstring lastExplaingString(aString.substr(lastPosition, aString.size() - lastPosition));
		myStrings.Add(lastExplaingString);
	}
}

inline std::wstring CTextInstance::GetText() const
{

	std::wstring string;
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

#include "stdafx.h"
#include "TextInstance.h"

#include "Renderer.h"

const CU::Vector4f CTextInstance::Red(1, 0, 0, 1);
const CU::Vector4f CTextInstance::Yellow(1, 1, 0, 1);
const CU::Vector4f CTextInstance::Green(0, 1, 0, 1);
const CU::Vector4f CTextInstance::Blue(0, 0, 1, 1);
const CU::Vector4f CTextInstance::Black(0, 0, 0, 1);
const CU::Vector4f CTextInstance::White(1, 1, 1, 1);

CTextInstance::CTextInstance()
	: myText(nullptr)
	  , myAlignemnt(eAlignment::LEFT), myColor(1, 1, 1, 1)
{
	myStrings.Init(2);
}

CTextInstance::CTextInstance(const CTextInstance& aTextInstance)
{
	*this = aTextInstance;
}

CTextInstance::~CTextInstance()
{
	//SAFE_DELETE(myText);
}

void CTextInstance::Init(const std::string& aFontPath)
{
	//maybe shouldn't be pointer
	myText = new CText(aFontPath);
	myLineGap.y = GetlineHeight();
}

void CTextInstance::Render() const
{
	if (myText != nullptr && myStrings.Size() > 0)
	{
		SRenderTextMessage* renderTextMessage = new SRenderTextMessage();
		renderTextMessage->myColor = myColor;
		renderTextMessage->myPosition = myPosition;
		renderTextMessage->myLineHeight = myLineGap + CU::Vector2f(0,GetlineHeight());
		renderTextMessage->myStrings = myStrings;
		renderTextMessage->myText = myText;
		renderTextMessage->myAlignement = myAlignemnt;
		CEngine::GetInstance()->GetRenderer().AddRenderMessage(renderTextMessage);
	}
}

bool CTextInstance::SetTextLine(const unsigned int aLineNumber, const std::string& aString)
{
	if (aLineNumber < myStrings.Size() && aLineNumber >= 0)
	{
		myStrings[aLineNumber] = aString;
		return true;
	}

	if (aLineNumber == myStrings.Size())
	{
		myStrings.Add(aString);
		return true;
	}

	return false;
}

bool CTextInstance::SetTextLine(const unsigned int aLineNumber, std::string&& aString)
{
	if (aLineNumber < myStrings.Size() && aLineNumber >= 0)
	{
		myStrings[aLineNumber] = std::move(aString);
	}

	if (aLineNumber > myStrings.Size())
	{
		for (int i = 0; i < aLineNumber - myStrings.Size() +1; ++i)
		{
			myStrings.Add("");
		}
	}

	if (aLineNumber == myStrings.Size())
	{
		myStrings.Add(std::move(aString));
		return true;
	}

	return false;
}

void CTextInstance::SetTextLines(const CU::GrowingArray<std::string>& someLines)
{
	myStrings = someLines;
}

void CTextInstance::SetTextLines(CU::GrowingArray<std::string>&& someLines)
{
	myStrings = std::move(someLines);
}

const CU::GrowingArray<std::string>& CTextInstance::GetTextLines()
{
	return myStrings;
}

float CTextInstance::GetlineHeight() const
{
	return myText->GetlineHeight();
}

CU::Vector2f CTextInstance::GetQuadSizeNormalized() const
{
	CU::Vector2f rectPixelSize;
	for (const std::string& str : myStrings)
	{
		CU::Vector2f stringSize(myText->CalculateRectPixelSize(str.c_str()));
		rectPixelSize.x = max(stringSize.x, rectPixelSize.x);
		rectPixelSize.y += stringSize.y;
	}

	rectPixelSize /= WINDOW_SIZE_F;
	return rectPixelSize;
}

void CTextInstance::SetAlignment(eAlignment anAlignment)
{
	myAlignemnt = anAlignment;
}

CTextInstance& CTextInstance::operator=(const CTextInstance& aTextInstance)
{
	if (myText != nullptr)
	{
		delete myText;
		myText = nullptr;
	}

	if (aTextInstance.myText != nullptr)
	{
		myText = new CText(*aTextInstance.myText);
	}

	myColor = aTextInstance.myColor;
	myPosition = aTextInstance.myPosition;
	myStrings = aTextInstance.myStrings;

	return *this;
}

#pragma once
#include "../CommonUtilities/DynamicString.h"
#include "../CommonUtilities/vector2.h"
#define Pling NewLine

class CTextInstance;

class CTextBox
{
public:
	CTextBox();
	~CTextBox();

	void AddText(CU::DynamicString myString);
	void NewLine();
	void RemoveLine();
	void Clear();

	void Render();

	void SetPosition(const CU::Vector2f &aPosition);
private:
	CU::Vector2f myPosition;
	CU::GrowingArray<CTextInstance*> myTextInstances;
};


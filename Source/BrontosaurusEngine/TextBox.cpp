#include "stdafx.h"
#include "TextBox.h"
#include "TextInstance.h"

CTextBox::CTextBox()
{
	CTextInstance* nextLine = new CTextInstance;
	nextLine->Init();
	nextLine->SetPosition(myPosition);
	myTextInstances.Init(2);
	myTextInstances.Add(nextLine);
}


CTextBox::~CTextBox()
{
	myTextInstances.DeleteAll();
}

void CTextBox::AddText(std::wstring myString)
{
	const std::wstring &currentText = myTextInstances[myTextInstances.Size()-1]->GetText();
	myTextInstances[myTextInstances.Size() -1]->SetText(currentText + myString);
}

void CTextBox::NewLine()
{
	const float lineHeight = myTextInstances[0]->GetlineHeight();
	CTextInstance* newTextInstance = new CTextInstance;
	newTextInstance->Init();
	newTextInstance->SetPosition({ myPosition.x , myPosition.y + lineHeight * myTextInstances.Size() });
	myTextInstances.Add(newTextInstance);
}

void CTextBox::RemoveLine()
{
	myTextInstances.RemoveAtIndex(myTextInstances.Size() - 1);
}

void CTextBox::Clear()
{
	myTextInstances.DeleteAll();
	CTextInstance* nextLine = new CTextInstance;
	nextLine->Init();
	nextLine->SetPosition(myPosition);
	myTextInstances.Add(nextLine);
}

void CTextBox::Render()
{
	for (unsigned int i = 0; i < myTextInstances.Size(); ++i)
	{
		myTextInstances[i]->Render();
	}
}

void CTextBox::SetPosition(const CU::Vector2f& aPosition)
{
	const CU::Vector2f positionDelta = aPosition - myPosition;
	for (unsigned int i = 0; i < myTextInstances.Size(); ++i)
	{
		CTextInstance& textInstance = *myTextInstances[i];
		textInstance.SetPosition(textInstance.GetPosition() + positionDelta);
	}

	myPosition = aPosition;
}

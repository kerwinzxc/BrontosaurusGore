#include "stdafx.h"
#include "C2DGUIRenderer.h"
#include "binary_tree.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "FullScreenHelper.h"
#include "Renderer.h"


C2DGUIRenderer::C2DGUIRenderer()
{
	myElements.Init(1);
	myRenderQueus.Init(1);

	myCurrentPackage = &myInputPackage;
}


C2DGUIRenderer::~C2DGUIRenderer()
{
}

void C2DGUIRenderer::CreateOrClearEvent(const std::wstring anElementName, const CU::Vector2ui& aPixelSize, const SGUIElement& aGuiElement)
{
	if (CheckIfElementIsCreated(anElementName) == false)
	{
		CreateNewElement(anElementName, aPixelSize, aGuiElement);
	}
	else
	{
		const int elementIndex = myElementIndexMap.at(anElementName);

		const CU::Vector2ui currentElementSize(myElements[elementIndex].second->GetSize());

		if (aPixelSize == currentElementSize)
		{
			myElements[elementIndex].second->Clear();
			myElements[elementIndex].first = aGuiElement;
		}
		else
		{
			myElements[elementIndex].second->ReInit(aPixelSize);
			myElements[elementIndex].first = aGuiElement;
		}
	}
}

void C2DGUIRenderer::CreateNewElement(const std::wstring& aName, const CU::Vector2ui& aPixelSize, const SGUIElement& aGUIElement)
{
	CRenderPackage* renderPackage = new CRenderPackage;
	renderPackage->Init(aPixelSize);

	myElementIndexMap[aName] = myElements.Size();
	myElements.Add(std::make_pair(aGUIElement, renderPackage));
	myRenderQueus.Add(CU::Queue<SRenderMessage*>());
}

bool C2DGUIRenderer::CheckIfElementIsCreated(std::wstring aName) const
{
	if (myElementIndexMap.count(aName) > 0)
	{
		return true;
	}

	return false;
}

void C2DGUIRenderer::ActivateElement(const std::wstring& aName)
{
	const unsigned char elementIndex = myElementIndexMap.at(aName);
	myElements.At(elementIndex).second->Activate();
}

void C2DGUIRenderer::RemoveElement(const std::wstring& anElementName)
{
	const unsigned char index = myElementIndexMap.at(anElementName);
	myElementIndexMap.erase(anElementName);
	delete myElements[index].second;
	myElements.RemoveAtIndex(index);
	myRenderQueus.RemoveAtIndex(index);

	for (auto elementIndex : myElementIndexMap)
	{
		if (elementIndex.second > index)
		{
			myElementIndexMap.at(elementIndex.first) -= 1;
		}
	}
}

void C2DGUIRenderer::RenderWholeGuiToPackage(CRenderPackage& aTargetPackage, CFullScreenHelper & aFullScreenHelper)
{
	aTargetPackage.Activate();

	const CU::Vector2f targetSize = aTargetPackage.GetSize();

	for (unsigned char i = 0; i < myElements.Size(); ++i)
	{
		const SGUIElement& currentGuiElement = myElements[i].first;

		const float newPixelWidth = currentGuiElement.myScreenRect.w * targetSize.y / (currentGuiElement.myScreenRect.w * 9 / (currentGuiElement.myScreenRect.z * 16));
		const float fullScreenWidth = newPixelWidth / targetSize.y;

		const CU::Vector2f screenSize(fullScreenWidth, currentGuiElement.myScreenRect.w);

		CU::Vector2f screenSpacePosition;

		if (currentGuiElement.myAnchor[static_cast<int>(eAnchors::eRight)])
		{
			screenSpacePosition.x = 1 - currentGuiElement.myScreenRect.x;
		}
		else
		{
			screenSpacePosition.y = currentGuiElement.myScreenRect.x;
		}

		if (currentGuiElement.myAnchor[static_cast<int>(eAnchors::eBottom)])
		{
			screenSpacePosition.y = 1 - currentGuiElement.myScreenRect.y;
		}
		else
		{
			screenSpacePosition.y = currentGuiElement.myScreenRect.y;	
		}

		screenSpacePosition.x -= screenSize.x * currentGuiElement.myOrigin.x;
		screenSpacePosition.y -= screenSize.y * currentGuiElement.myOrigin.y;

		const CU::Vector4f screenRect(screenSpacePosition.x, screenSpacePosition.y, screenSpacePosition.x + screenSize.x, screenSpacePosition.y + screenSize.y);

		aFullScreenHelper.DoEffect(CFullScreenHelper::eEffectType::eCopy, screenRect, myElements[i].second);
	}
}

void C2DGUIRenderer::DoRenderQueues(CRenderer & aRenderer, int & drawCallsCount)
{
	for (unsigned char i = 0; i < myRenderQueus.Size(); ++i)
	{
		if (myRenderQueus[i].Size() == 0)
		{
			continue;
		}

		myElements[i].second->Activate();
		myCurrentPackage = myElements[i].second;
		while (myRenderQueus[i].Size() > 0)
		{
			SRenderMessage* const renderMessage = myRenderQueus[i].Pop();
			aRenderer.HandleRenderMessage(renderMessage, drawCallsCount);
			//delete renderMessage;
		}
	}
	myCurrentPackage = &myInputPackage;
}

void C2DGUIRenderer::RenderToGUI(const std::wstring anElementName, SRenderMessage* aRenderMessage)
{
	if (aRenderMessage != nullptr)
	{
		myRenderQueus[myElementIndexMap.at(anElementName)].Push(aRenderMessage);
	}
}

CRenderPackage& C2DGUIRenderer::GetCurrentPackage()
{
	return *myCurrentPackage;
}

CRenderPackage& C2DGUIRenderer::GetInputPackage()
{
	return myInputPackage;
}

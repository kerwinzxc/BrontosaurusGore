#pragma once
#include "RenderPackage.h"
#include "../ThreadedPostmaster/Subscriber.h"
#include <unordered_map>
#include "BitSet.h"
#include "../GUI/GUIElement.h"
#include "Queue.h"


class CSprite;
class CRenderer; 
struct SRenderMessage;

class C2DGUIRenderer: Postmaster::ISubscriber
{
public:
	C2DGUIRenderer();
	~C2DGUIRenderer();

	void CreateOrClearEvent(const std::wstring anElementName, const CU::Vector2ui& aPixelSize,const SGUIElement & aGuiElement);
	void CreateNewElement(const std::wstring& aName, const CU::Vector2ui& aPixelSize, const SGUIElement& aGUIElement);
	bool CheckIfElementIsCreated(std::wstring aName) const;
	void ActivateElement(const std::wstring& aName);
	void RemoveElement(const std::wstring& anElementName);

	void RenderWholeGuiToPackage(CRenderPackage& aTargetPackage, CFullScreenHelper& aFullScreenHelper);

	void DoRenderQueues(CRenderer& aRenderer, int& drawCallsCount);
	void RenderToGUI(const std::wstring anElementName, SRenderMessage* aRenderMessage);
	
	CRenderPackage& GetCurrentPackage();
	CRenderPackage& GetInputPackage();

private:
	std::unordered_map<std::wstring,unsigned char> myElementIndexMap;
	CU::GrowingArray<std::pair<SGUIElement, CRenderPackage*>,unsigned char> myElements;
	CU::GrowingArray<CU::Queue<SRenderMessage*>, unsigned char> myRenderQueus;

	CRenderPackage* myCurrentPackage;
	CRenderPackage myInputPackage;
};


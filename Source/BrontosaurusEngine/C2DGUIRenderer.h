#pragma once
#include "RenderPackage.h"
#include "../ThreadedPostmaster/Subscriber.h"
#include <unordered_map>
#include "../GUI/GUIElement.h"
#include "Queue.h"
#include "Colour.h"
#include <atomic>


class CSprite;
class CRenderer; 
struct SRenderMessage;
struct SRenderBarMessage;

class C2DGUIRenderer: Postmaster::ISubscriber
{
public:
	C2DGUIRenderer();
	~C2DGUIRenderer();

	void InitBars();

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

	void RenderBar(const SRenderBarMessage* const aRenderMessage);

	void Clear();
	void QueuClear();
private:

	std::unordered_map<std::wstring,unsigned char> myElementIndexMap;
	CU::GrowingArray<std::pair<SGUIElement, CRenderPackage*>,unsigned char> myElements;
	CU::GrowingArray<CU::Queue<SRenderMessage*>, unsigned char> myRenderQueus;

	CRenderPackage* myCurrentPackage;
	CRenderPackage myInputPackage;

	struct
	{
		ID3D11Buffer* myVSConstBuffer;
		ID3D11Buffer* myVertexBuffer;
		ID3D11Buffer* myPSConstBuffer;

		CEffect* myEffect;

		bool myIsInited;
	}myBarData;

	std::atomic_bool myShouldClear;
};


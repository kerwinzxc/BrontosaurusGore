#include "stdafx.h"
#include "C2DGUIRenderer.h"
#include "binary_tree.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "FullScreenHelper.h"
#include "Renderer.h"
#include "ConstBufferTemplate.h"
#include "EModelBluePrint.h"
#include "ShaderManager.h"

struct SBarVSData
{
	CU::Vector2f position;
	CU::Vector2f size;
};

struct SBarPSData
{
	CU::Colour fullColour;
	CU::Colour emptyColour;
	CU::Colour backgroundColour;
	float myState;
	CU::Vector3f TRASH;
};

C2DGUIRenderer::C2DGUIRenderer()
{
	myElements.Init(1);
	myRenderQueus.Init(1);

	myCurrentPackage = &myInputPackage;

	myBarData.myIsInited = false;
	myBarData.myVSConstBuffer = nullptr;
	myBarData.myPSConstBuffer = nullptr;
	myBarData.myVertexBuffer = nullptr;
}


C2DGUIRenderer::~C2DGUIRenderer()
{
}

void C2DGUIRenderer::InitBars()
{
	CU::Vector4f vertices[7] =
	{
		CU::Vector4f(0.0f, 0.0f, -0.50f, 1.0f),
		CU::Vector4f(0.0f, 0.0f, -0.50f, 1.0f),
		CU::Vector4f(0.0f, 1.0f, -0.50f, 1.0f),
		CU::Vector4f(1.0f, 1.0f, -0.50f, 1.0f),
		CU::Vector4f(0.0f, 0.0f, -0.50f, 1.0f),
		CU::Vector4f(1.0f, 1.0f, -0.50f, 1.0f),
		CU::Vector4f(1.0f, 0.0f, -0.50f, 1.0f)
		//topLeft, botLeft, botRight, botRight, topRight, topLeft
	};

	// VERTEX BUFFER
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_IMMUTABLE;
	vertexBufferDesc.ByteWidth = sizeof(vertices);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexData = {};
	vertexData.pSysMem = vertices;
	HRESULT result = FRAMEWORK->GetDevice()->CreateBuffer(&vertexBufferDesc, &vertexData, &myBarData.myVertexBuffer);
	CHECK_RESULT(result, "Failed to create vertexbuffer.");

	//Create VSConstantBuffer
	SBarVSData vSBufferType;
	myBarData.myVSConstBuffer = BSR::CreateCBuffer<SBarVSData>(&vSBufferType);

	//Create PSConstentBuffer
	SBarPSData pSBuffertype;
	myBarData.myPSConstBuffer = BSR::CreateCBuffer(&pSBuffertype);

	//Effect
	unsigned int shadeBlueprint = EModelBluePrint_Bar;

	ID3D11VertexShader* vertexShader = SHADERMGR->LoadVertexShader(L"Shaders/bar_shader.fx", shadeBlueprint);
	ID3D11PixelShader* pixelShader = SHADERMGR->LoadPixelShader(L"Shaders/bar_shader.fx", shadeBlueprint);
	ID3D11InputLayout* inputLayout = SHADERMGR->LoadInputLayout(L"Shaders/bar_shader.fx", shadeBlueprint);

	myBarData.myEffect = new CEffect(vertexShader, pixelShader, nullptr, inputLayout, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	myBarData.myIsInited = true;
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
	if (myElementIndexMap.count(anElementName) > 0)
	{
		if (aRenderMessage != nullptr)
		{
			myRenderQueus[myElementIndexMap.at(anElementName)].Push(aRenderMessage);
		}
	}
	else
	{
		DL_ASSERT("GUI Element missing");
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

void C2DGUIRenderer::RenderBar(const SRenderBarMessage* const aRenderMessage)
{
	if (myBarData.myIsInited == false)
	{
		InitBars();
	}

	myBarData.myEffect->Activate();

	ID3D11DeviceContext& context = *CEngine::GetInstance()->GetFramework()->GetDeviceContext();

	//VertexShader Constant buffer
	CU::Vector4f rect = aRenderMessage->myRect;

	SBarVSData barVsData;
	barVsData.position = CU::Vector2f(rect.x, rect.y);
	barVsData.size = CU::Vector2f(rect.z - rect.x, rect.w - rect.y);

	D3D11_MAPPED_SUBRESOURCE mappedSubresource = {};

	context.Map(myBarData.myVSConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, &barVsData, sizeof(SBarVSData));
	context.Unmap(myBarData.myVSConstBuffer, 0);

	context.VSSetConstantBuffers(1, 1, &myBarData.myVSConstBuffer);

	//PixelShader constant buffer
	SBarPSData barPsData;
	barPsData.myState = aRenderMessage->myCurrentLevel;
	barPsData.backgroundColour = aRenderMessage->myBackgroundColour;
	barPsData.fullColour = aRenderMessage->myFullColour;
	barPsData.emptyColour = aRenderMessage->myEmptyColour;

	mappedSubresource = {};

	context.Map(myBarData.myPSConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, &barPsData, sizeof(barPsData));
	context.Unmap(myBarData.myPSConstBuffer, 0);

	context.PSSetConstantBuffers(2, 1, &myBarData.myPSConstBuffer);

	UINT stride = sizeof(CU::Vector4f);
	UINT offset = 0;
	FRAMEWORK->GetDeviceContext()->IASetVertexBuffers(0, 1, &myBarData.myVertexBuffer, &stride, &offset);
	FRAMEWORK->GetDeviceContext()->Draw(6, 1);
}

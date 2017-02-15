#include "stdafx.h"
#include "GUIManager.h"

#include "WidgetContainer.h"
#include "WidgetFactory.h"
#include "GUICursor.h"

#include "../PostMaster/PostMaster.h"
#include "../PostMaster/Event.h"
#include "../PostMaster/Message.h"

#include "../BrontosaurusEngine/Engine.h"
#include "../BrontosaurusEngine/Renderer.h"

#include "../Audio/AudioInterface.h"

static CU::Vector2f locMousePosition;


GUI::GUIManager::GUIManager()
	: myWidgetContainer(nullptr)
	, myFocusedWidget(nullptr)
	, myWidgetAtMouse(nullptr)
	, myCursor(nullptr)
	, myShouldUpdate(true)
	, myShouldRender(true)
	, myCamera(nullptr)
	, myShouldRenderMouse(true)
{
}

GUI::GUIManager::~GUIManager()
{
	PostMaster::GetInstance().UnSubscribe(this, eMessageType::eMouseMessage);
	PostMaster::GetInstance().UnSubscribe(this, eMessageType::eKeyboardMessage);

	myFocusedWidget = nullptr;
	myWidgetAtMouse = nullptr;
	SAFE_DELETE(myWidgetContainer);

	PostMaster::GetInstance().UnSubscribe(myCursor, eMessageType::eMouseMessage);
	PostMaster::GetInstance().UnSubscribe(myCursor, eMessageType::eSecretMouseMessageMvhCarl);
	SAFE_DELETE(myCursor);
	SAFE_DELETE(myCamera);
}

void GUI::GUIManager::Init()
{
	myWidgetContainer = new WidgetContainer(CU::Vector2f::Zero, CU::Vector2f::One, "BaseWidget", true);

	myCursor = new GUICursor();
	PostMaster::GetInstance().Subscribe(myCursor, eMessageType::eMouseMessage, 6);
	PostMaster::GetInstance().Subscribe(myCursor, eMessageType::eSecretMouseMessageMvhCarl);
}

void GUI::GUIManager::Init(const char* aGUIScenePath)
{
	myWidgetContainer = WidgetFactory::CreateGUIScene(aGUIScenePath, myCamera);

	myCursor = new GUICursor();
	PostMaster::GetInstance().Subscribe(myCursor, eMessageType::eMouseMessage, 6);
	PostMaster::GetInstance().Subscribe(myCursor, eMessageType::eSecretMouseMessageMvhCarl);
}

#include "ToolTipDecorator.h"
void GUI::GUIManager::Update(const CU::Time& aDeltaTime)
{
	if (myShouldUpdate == true)
	{
		//get skills from pollingstation and set icons to visible if player have them
		myWidgetContainer->Update(aDeltaTime);

		locMousePosition = myCursor->GetPosition();
	}
}

void GUI::GUIManager::Render()
{
	SChangeStatesMessage* changeStateMessage = new SChangeStatesMessage();
	changeStateMessage->myDepthStencilState = eDepthStencilState::eDisableDepth;
	changeStateMessage->myRasterizerState = eRasterizerState::eDefault;
	changeStateMessage->myBlendState = eBlendState::eAlphaBlend;
	changeStateMessage->mySamplerState = eSamplerState::eWrap;
	RENDERER.AddRenderMessage(changeStateMessage);

	if (myCamera != nullptr)
	{
		SSetCameraMessage* setCameraMessage = new SSetCameraMessage();
		setCameraMessage->myCamera = *myCamera;
		RENDERER.AddRenderMessage(setCameraMessage);
	}

	myWidgetContainer->Render();
	myWidgetContainer->RenderFrontLayers();

	if (myShouldRenderMouse == true)
	{
		myCursor->Render();
	}
}

void GUI::GUIManager::AddWidget(const std::string& aName, IWidget* aWidget)
{
	if (aName.empty() == true)
	{
		assert(!"Tried to add widget without a name to GUI manager");
		return;
	}
	if (aWidget == nullptr)
	{
		assert(!"Tried to add null-widget to GUI manager");
		return;
	}

	myWidgetContainer->AddWidget(aName, aWidget);
}

void GUI::GUIManager::DeleteWidget(const std::string& aName)
{
	IWidget* widget = myWidgetContainer->RemoveWidget(aName);
	SAFE_DELETE(widget);
}

GUI::IWidget* GUI::GUIManager::RemoveWidget(const std::string& aName)
{
	if (aName.empty() == true)
	{
		assert(!"Tried to remove widget with empty name in GUI manager");
		return nullptr;
	}

	return myWidgetContainer->RemoveWidget(aName);
}

GUI::IWidget* GUI::GUIManager::FindWidget(const std::string& aName)
{
	if (aName.empty() == true)
	{
		assert(!"Tried to remove widget with empty name in GUI manager");
		return nullptr;
	}

	return myWidgetContainer->FindWidget(aName);
}

void GUI::GUIManager::PauseUpdate()
{
}

void GUI::GUIManager::PauseRender()
{
}

void GUI::GUIManager::RestartUpdate()
{
}

void GUI::GUIManager::RestartRender()
{
}

eMessageReturn GUI::GUIManager::MouseClicked(const CU::eMouseButtons aMouseButton, const CU::Vector2f& aMousePosition)
{
	SUPRESS_UNUSED_WARNING(aMousePosition);
	CU::Vector2f mousePosition = myCursor->GetPosition();

	IWidget* widget = myWidgetContainer->MouseIsOver(mousePosition);

	if (myFocusedWidget != nullptr && myFocusedWidget != widget)
	{
		myFocusedWidget->OnLostFocus();
	}

	myFocusedWidget = widget;

	if (myFocusedWidget == nullptr || myFocusedWidget == myWidgetContainer)
	{
		return eMessageReturn::eContinue;
	}
	else
	{
		//Audio::CAudioInterface::GetInstance()->PostEvent("ButtonClick");
		myFocusedWidget->OnGotFocus();
		myFocusedWidget->OnMousePressed(mousePosition, aMouseButton);

		return eMessageReturn::eStop;
	}
}

eMessageReturn GUI::GUIManager::MouseReleased(const CU::eMouseButtons aMouseButton, const CU::Vector2f& aMousePosition)
{
	SUPRESS_UNUSED_WARNING(aMousePosition);
	CU::Vector2f mousePosition = myCursor->GetPosition();

	IWidget* widget = myWidgetContainer->MouseIsOver(mousePosition);
	bool stoleInput = false;

	if (myFocusedWidget != nullptr && myFocusedWidget != widget)
	{
		stoleInput = true;
		myFocusedWidget->OnMouseReleased(mousePosition, aMouseButton);
	}
	if (widget != nullptr && widget != myWidgetContainer)
	{
		stoleInput = true;
		widget->OnMouseReleased(mousePosition, aMouseButton);
	}

	return (stoleInput == true) ? eMessageReturn::eStop : eMessageReturn::eContinue;
}

eMessageReturn GUI::GUIManager::MouseMoved(const CU::Vector2f& aMousePosition)
{
	SUPRESS_UNUSED_WARNING(aMousePosition);
	CU::Vector2f mousePosition = myCursor->GetPosition();

	IWidget* widget = myWidgetContainer->MouseIsOver(mousePosition);



	if (widget == myWidgetAtMouse)
	{
		if (myWidgetAtMouse != nullptr && myWidgetAtMouse != myWidgetContainer)
		{
			myWidgetAtMouse->OnMouseMove(mousePosition);
		}
	}
	else
	{
		if (myWidgetAtMouse != nullptr && myWidgetAtMouse != myWidgetContainer)
		{
			myWidgetAtMouse->OnMouseExit(mousePosition);
		}
		if (widget != nullptr && widget != myWidgetContainer)
		{
			DL_PRINT("mouse entered %s", widget->GetName().c_str());
			widget->OnMouseEnter(mousePosition);
		}

		myWidgetAtMouse = widget;
	}

	return eMessageReturn::eContinue;
}

eMessageReturn GUI::GUIManager::MouseDragged(const CU::eMouseButtons aMouseButton, const CU::Vector2f& aMousePosition, const CU::Vector2f& aLastMousePosition)
{
	IWidget* widget = myWidgetContainer->MouseIsOver(aMousePosition);
	if (widget != nullptr && widget != myWidgetContainer)
	{
		widget->OnMouseDrag(aMousePosition - aLastMousePosition, aMouseButton);
		return eMessageReturn::eStop;
	}

	return eMessageReturn::eContinue;
}

eMessageReturn GUI::GUIManager::Recieve(const Message& aMessage)
{
	return aMessage.myEvent.DoEvent(this);
}

void GUI::GUIManager::PauseRenderAndUpdate()
{
	PostMaster::GetInstance().UnSubscribe(this, eMessageType::ePlayerGotHat);
	PostMaster::GetInstance().UnSubscribe(this, eMessageType::eMouseMessage);
	PostMaster::GetInstance().UnSubscribe(this, eMessageType::eKeyboardMessage);
	PostMaster::GetInstance().UnSubscribe(myCursor, eMessageType::eMouseMessage);
	PostMaster::GetInstance().UnSubscribe(myCursor, eMessageType::eSecretMouseMessageMvhCarl);

	myShouldUpdate = false;
	myShouldRender = false;

	if (myWidgetContainer != nullptr)
	{
		myWidgetContainer->SetVisibility(false);
		myWidgetContainer->RemoveDebugLines();
	}
	
}

void GUI::GUIManager::RestartRenderAndUpdate(const bool aSubscribeToSecretMessage, const bool aAddSecretHats)
{
	myShouldUpdate = true;
	myShouldRender = true;

	myWidgetContainer->SetVisibility(true);

	PostMaster::GetInstance().Subscribe(this, eMessageType::ePlayerGotHat);
	PostMaster::GetInstance().Subscribe(this, eMessageType::eMouseMessage, 5);
	PostMaster::GetInstance().Subscribe(this, eMessageType::eKeyboardMessage, 5);
	PostMaster::GetInstance().Subscribe(myCursor, eMessageType::eMouseMessage, 6);

	if (aSubscribeToSecretMessage)
	{
		PostMaster::GetInstance().Subscribe(myCursor, eMessageType::eSecretMouseMessageMvhCarl);
	}


	if (locMousePosition != CU::Vector2f::Zero && myCursor != nullptr)
	{
		myCursor->SetPosition(locMousePosition);
	}
}


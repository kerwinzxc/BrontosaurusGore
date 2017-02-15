#include "stdafx.h"
#include "InputManager.h"

#include "Engine.h"
#include "WindowsWindow.h"

#include "../CommonUtilities/EKeyboardKeys.h"
#include "../CommonUtilities/EMouseButtons.h"
#include "../CommonUtilities/EInputMessage.h"
#include "../CommonUtilities/InputWrapper.h"
#include "../CommonUtilities/XInputWrapper.h"
#include "../CommonUtilities/vector2.h"

#include "../PostMaster/PostMaster.h"
#include "../PostMaster/Message.h"
#include "../PostMaster/MouseClicked.h"
#include "../PostMaster/MouseReleased.h"
#include "../PostMaster/MouseMoved.h"
#include "../PostMaster/KeyPressed.h"
#include "../PostMaster/KeyReleased.h"
#include "../PostMaster//InputMessagePressed.h"

CInputManager::CInputManager()
	: myDInputWrapper(nullptr)
	, myXInputWrapper(nullptr)
{
	myDInputWrapper = new CU::InputWrapper();

	HINSTANCE hingsten = CEngine::GetInstance()->GetWindow()->GetHinstance();
	HWND hunden = CEngine::GetInstance()->GetWindow()->GetHWND();

	/*bool directInputSuccess =*/ myDInputWrapper->Init(hingsten, hunden);

	myXInputWrapper = new CU::XInputWrapper();
	myXInputWrapper->Init(4);

	myDInputWrapper->Init(hingsten, hunden);
	myKeyDowns.Init(20);

	CU::Vector2f windowSize(WINDOW_SIZE);
	CU::Vector2f middleOfWindow = windowSize / 2.f;
	myLastMousePosition = middleOfWindow;

	myHasFocus = true;

	PostMaster::GetInstance().Subscribe(this, eMessageType::eFocusChanged);
}

CInputManager::~CInputManager()
{
	SAFE_DELETE(myDInputWrapper);
	SAFE_DELETE(myXInputWrapper);
	PostMaster::GetInstance().UnSubscribe(this, eMessageType::eFocusChanged);
}

eMessageReturn CInputManager::Recieve(const Message& aMessage)
{
	return aMessage.myEvent.DoEvent(this);
}

void CInputManager::SetMousePosition(const CU::Vector2f& aMousePosition)
{
	myDInputWrapper->SetMousePosition(static_cast<int>(aMousePosition.x), static_cast<int>(aMousePosition.y));
}

void CInputManager::LockUnlockMouse(const bool aHasFocus)
{
	myHasFocus = aHasFocus;
	ShowCursor(!aHasFocus);
}

void CInputManager::Update()
{
	myDInputWrapper->Update();
	myXInputWrapper->UpdateStates();

	UpdateMouse();
	UpdateKeyboard();
	UpdateGamePad();
}

void CInputManager::UpdateMouse()
{
	if (myHasFocus == true)
	{
		CU::Vector2i windowSize(WINDOW_SIZE);
		CU::Vector2i middleOfWindow = windowSize / 2;
		CU::Vector2i newWindowsMousePos(myDInputWrapper->GetMousePositionX(), myDInputWrapper->GetMousePositionY());

		CU::Vector2f mouseDelta(newWindowsMousePos - middleOfWindow);
		CU::Vector2f mousePosition = myLastMousePosition + mouseDelta;

		mousePosition.Clamp(CU::Vector2f::Zero, CU::Vector2f(windowSize));

		if (myLastMousePosition != mousePosition)
		{
			PostMaster::GetInstance().SendLetter(Message(eMessageType::eMouseMessage, MouseMoved(mousePosition)));
			myLastMousePosition = mousePosition;
		}
		if (myDInputWrapper->IsMouseButtonPressed(CU::eMouseButtons::LBUTTON) == true)
		{
			PostMaster::GetInstance().SendLetter(Message(eMessageType::eMouseMessage, MouseClicked(mousePosition, CU::eMouseButtons::LBUTTON)));
		}
		if (myDInputWrapper->IsMouseButtonReleased(CU::eMouseButtons::LBUTTON) == true)
		{
			PostMaster::GetInstance().SendLetter(Message(eMessageType::eMouseMessage, MouseReleased(mousePosition, CU::eMouseButtons::LBUTTON)));
		}
		if (myDInputWrapper->IsMouseButtonPressed(CU::eMouseButtons::RBUTTON) == true)
		{
			PostMaster::GetInstance().SendLetter(Message(eMessageType::eMouseMessage, MouseClicked(mousePosition, CU::eMouseButtons::RBUTTON)));
		}
		if (myDInputWrapper->IsMouseButtonReleased(CU::eMouseButtons::RBUTTON) == true)
		{
			PostMaster::GetInstance().SendLetter(Message(eMessageType::eMouseMessage, MouseReleased(mousePosition, CU::eMouseButtons::RBUTTON)));
		}

		myDInputWrapper->SetMousePosition(middleOfWindow.x, middleOfWindow.y); // drar 1/3 av all prestanda på maintråden.
	}
}

void CInputManager::UpdateKeyboard()
{
	if (myDInputWrapper->GetKeysPressed(myKeyDowns) == true)
	{
		for (unsigned int i = 0; i < myKeyDowns.Size(); ++i)
		{
			PostMaster::GetInstance().SendLetter(Message(eMessageType::eKeyboardMessage, KeyPressed(myKeyDowns[i])));
			//PostMaster::GetInstance().SendLetter(Message(eMessageType::eKeyPressed, KeyPressed(myKeyDowns[i])));
			PostMaster::GetInstance().SendLetter(Message(eMessageType::eInputMessagePressed, InputMessagePressed(static_cast<CU::eInputMessage>(myKeyDowns[i]))));
		}
	}

	if (myDInputWrapper->GetKeysReleased(myKeyDowns) == true)
	{
		for (unsigned int i = 0; i < myKeyDowns.Size(); ++i)
		{
			PostMaster::GetInstance().SendLetter(Message(eMessageType::eKeyboardMessage, KeyReleased(myKeyDowns[i])));
		}
	}
}

void CInputManager::UpdateGamePad()
{
	for (unsigned i = 0; i < myXInputWrapper->GetConnectedJoystickCount(); ++i)
	{
		if (myXInputWrapper->GetKeyPressed(0, CU::XInputWrapper::GAMEPAD::A) == true)
		{
			DL_PRINT("'a' pressed!");
		}
	}
}

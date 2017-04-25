#include "stdafx.h"
#include "InputManager.h"

#include "Engine.h"
#include "WindowsWindow.h"

#include "../CommonUtilities/InputWrapper.h"
#include "../CommonUtilities/XInputWrapper.h"
#include "../CommonUtilities/EKeyboardKeys.h"
#include "../CommonUtilities/EMouseButtons.h"
#include "../CommonUtilities/EInputMessage.h"

#include "../PostMaster/PostMaster.h"
#include "../PostMaster/Message.h"
#include "../PostMaster/Event.h"

#include "../CommonUtilities/InputMessenger.h"
#include "../CommonUtilities/InputMessage.h"
#include "../CommonUtilities/EInputReturn.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../PostMaster/FocusChange.h"

CInputManager* CInputManager::ourInstance = nullptr;

CInputManager::CInputManager()
	: myKeys(20)
	, myMessengers(8u)
	, myDInputWrapper(nullptr)
	, myXInputWrapper(nullptr)
{
	assert(ourInstance == nullptr && "Should not create two input managers");
	ourInstance = this;

	myDInputWrapper = new CU::InputWrapper();

	HINSTANCE hingsten = CEngine::GetInstance()->GetWindow()->GetHinstance();
	HWND hunden = CEngine::GetInstance()->GetWindow()->GetHWND();

	/*bool directInputSuccess =*/ myDInputWrapper->Init(hingsten, hunden);

	myXInputWrapper = new CU::XInputWrapper();
	myXInputWrapper->Init(4);

	myDInputWrapper->Init(hingsten, hunden);

	CU::Vector2f windowSize(WINDOW_SIZE);
	CU::Vector2f middleOfWindow = windowSize / 2.f;
	myLastMousePosition = middleOfWindow;

	myHasFocus = true;

	Postmaster::Threaded::CPostmaster::GetInstance().Subscribe(this, eMessageType::eFocusChanged);
}

CInputManager::~CInputManager()
{
	assert(ourInstance != nullptr && "Missed setting ourInstance to this in constructor?");
	assert(ourInstance == this && "Should not create two input managers");
	ourInstance = nullptr;

	SAFE_DELETE(myDInputWrapper);
	SAFE_DELETE(myXInputWrapper);

	Postmaster::Threaded::CPostmaster::GetInstance().Unsubscribe(this);
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

void CInputManager::Listen(CU::CInputMessenger& aMessenger, const int aPriority)
{
	if (aPriority > 0)
	{
		for (unsigned int i = 0; i < myMessengers.Size(); ++i)
		{
			if (myMessengers[i]->GetPriority() < aPriority)
			{
				myMessengers.Insert(i, &aMessenger);
				return;
			}
		}
	}

	myMessengers.Add(&aMessenger);
}

void CInputManager::Neglect(CU::CInputMessenger& aMessenger)
{
	myMessengers.Remove(&aMessenger);
}

CInputManager* CInputManager::GetInstance()
{
	return ourInstance;
}

eMessageReturn CInputManager::DoEvent(const FocusChange& aFocusChange)
{
	LockUnlockMouse(aFocusChange.GetHasFocus());
	return eMessageReturn::eContinue;
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

		static CU::Vector2i windowOffset;
		static bool locGetRect = true;
		if (locGetRect)
		{
			locGetRect = false;
			RECT rect;
			GetWindowRect(GetFocus(), &rect);
			windowOffset.x = rect.left;
			windowOffset.y = rect.top;
		}

		CU::Vector2i middleOfWindow = windowSize / 2 + windowOffset;
		CU::Vector2i newWindowsMousePos(myDInputWrapper->GetMousePositionX(), myDInputWrapper->GetMousePositionY());

		CU::Vector2f windowSizeF(windowSize);

		CU::Vector2f mouseDelta(newWindowsMousePos - middleOfWindow);
		CU::Vector2f mouseDeltaNormalized(mouseDelta / windowSizeF);

		CU::Vector2f mousePosition(myLastMousePosition + mouseDeltaNormalized);
		mousePosition.Clamp(CU::Vector2f::Zero, CU::Vector2f::One);

		myLastMousePosition = mousePosition;

		if (mouseDeltaNormalized != CU::Vector2f::Zero)
		{
			CU::SInputMessage mouseMoved;
			mouseMoved.myType = CU::eInputType::eMouseMoved;
			mouseMoved.myMouseDelta = mouseDeltaNormalized;

			for (CU::CInputMessenger* messenger : myMessengers)
			{
				if (messenger->RecieveInput(mouseMoved) == CU::eInputReturn::eKeepSecret)
				{
					break;
				}
			}
		}

		for (int i = 0; i < static_cast<int>(CU::eMouseButtons::LENGTH); ++i)
		{
			CU::eMouseButtons mouseButton = static_cast<CU::eMouseButtons>(i);
			if (myDInputWrapper->IsMouseButtonPressed(mouseButton) == true)
			{
				CU::SInputMessage mousePressed;
				mousePressed.myType = CU::eInputType::eMousePressed;
				mousePressed.myMousePosition = myLastMousePosition;
				mousePressed.myMouseButton = mouseButton;

				for (CU::CInputMessenger* messenger : myMessengers)
				{
					if (messenger->RecieveInput(mousePressed) == CU::eInputReturn::eKeepSecret)
					{
						break;
					}
				}

			}
			if (myDInputWrapper->IsMouseButtonReleased(static_cast<CU::eMouseButtons>(i)) == true)
			{
				CU::SInputMessage mouseReleased;
				mouseReleased.myType = CU::eInputType::eMouseReleased;
				mouseReleased.myMousePosition = myLastMousePosition;
				mouseReleased.myMouseButton = mouseButton;

				for (CU::CInputMessenger* messenger : myMessengers)
				{
					if (messenger->RecieveInput(mouseReleased) == CU::eInputReturn::eKeepSecret)
					{
						break;
					}
				}
			}
		}

		int mouseWheelPosition = myDInputWrapper->GetMouseWheelPos();
		if (mouseWheelPosition != myLastMouseWheelPosition)
		{
			if (mouseWheelPosition != 0)
			{
				CU::SInputMessage mouseWheelChanged;
				mouseWheelChanged.myMouseWheelDelta.x = myLastMouseWheelPosition - mouseWheelPosition;
				mouseWheelChanged.myMouseWheelDelta.y = myLastMouseWheelPosition - mouseWheelPosition;
				mouseWheelChanged.myType = CU::eInputType::eScrollWheelChanged;

				for (CU::CInputMessenger* messenger : myMessengers)
				{
					if (messenger->RecieveInput(mouseWheelChanged) == CU::eInputReturn::eKeepSecret)
					{
						break;
					}
				}
			}

			myLastMouseWheelPosition = mouseWheelPosition;
		}

		myDInputWrapper->SetMousePosition(middleOfWindow.x, middleOfWindow.y);
	}
}

void CInputManager::UpdateKeyboard()
{
	if (myDInputWrapper->GetKeysPressed(myKeys) == true)
	{
		for (unsigned int i = 0; i < myKeys.Size(); ++i)
		{
			CU::SInputMessage keyPressed;
			keyPressed.myType = CU::eInputType::eKeyboardPressed;
			keyPressed.myKey = myKeys[i];

			for (CU::CInputMessenger* messenger : myMessengers)
			{
				if (messenger->RecieveInput(keyPressed) == CU::eInputReturn::eKeepSecret)
				{
					break;
				}
			}
		}
	}

	if (myDInputWrapper->GetKeysReleased(myKeys) == true)
	{
		for (unsigned int i = 0; i < myKeys.Size(); ++i)
		{
			CU::SInputMessage keyReleased;
			keyReleased.myType = CU::eInputType::eKeyboardReleased;
			keyReleased.myKey = myKeys[i];

			for (CU::CInputMessenger* messenger : myMessengers)
			{
				if (messenger->RecieveInput(keyReleased) == CU::eInputReturn::eKeepSecret)
				{
					break;
				}
			}
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

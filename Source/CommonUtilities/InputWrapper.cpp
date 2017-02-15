#include "stdafx.h"
#include "InputWrapper.h"

#include "EMouseButtons.h"
#include "EKeyboardKeys.h"

namespace CU

#define KEYBOARDKEYDOWN(key, name) (name[static_cast<int>(key)] & 0x80)
#define KEYBOARDKEYUP(key, name) (!(name[static_cast<int>(key)] & 0x80))
#define MOUSEBUTTONDOWN(button, name) (name.rgbButtons[static_cast<int>(button)] & 0x80)
#define MOUSEBUTTONUP(button, name) (!(name.rgbButtons[static_cast<int>(button)] & 0x80))

{
	InputWrapper::InputWrapper()
	{
		myDirectInputInterface = 0;
		myKeyboard = 0;
		myMouse = 0;
		myIsInitialized = false;

		ZeroMemory(&myKeyboardState, sizeof(char) * 256);
		ZeroMemory(&myPreviousKeyboardState, sizeof(char) * 256);
		ZeroMemory(&myMouseState, sizeof(myMouseState));
		ZeroMemory(&myPreviousMouseState, sizeof(myPreviousMouseState));
	}


	InputWrapper::~InputWrapper()
	{

	}

	bool InputWrapper::Init(HINSTANCE hinstance, HWND hwnd)
	{
		if (myIsInitialized == false)
		{
			HRESULT result;

			// Initialize the main direct input interface.
			result = DirectInput8Create(hinstance, DIRECTINPUT_VERSION, IID_IDirectInput8, (void**)&myDirectInputInterface, NULL);
			if (FAILED(result) == true)
			{
				return false;
			}

			// Initialize the direct input interface for the keyboard.
			result = myDirectInputInterface->CreateDevice(GUID_SysKeyboard, &myKeyboard, NULL);
			if (FAILED(result) == true)
			{
				return false;
			}

			// Set the data format.  In this case since it is a keyboard we can use the predefined data format.
			result = myKeyboard->SetDataFormat(&c_dfDIKeyboard);
			if (FAILED(result) == true)
			{
				return false;
			}

			// Set the cooperative level of the keyboard to not share with other programs.
			result = myKeyboard->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			if (FAILED(result) == true)
			{
				return false;
			}

			// Initialize the direct input interface for the mouse.
			result = myDirectInputInterface->CreateDevice(GUID_SysMouse, &myMouse, NULL);
			if (FAILED(result) == true)
			{
				return false;
			}

			// Set the data format for the mouse using the pre-defined mouse data format.
			result = myMouse->SetDataFormat(&c_dfDIMouse);
			if (FAILED(result) == true)
			{
				return false;
			}

			// Set the cooperative level of the mouse to share with other programs.
			result = myMouse->SetCooperativeLevel(hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			if (FAILED(result) == true)
			{
				return false;
			}

			myIsInitialized = true;

			// Now acquire the keyboard.
			result = myKeyboard->Acquire();
			if (FAILED(result) == true)
			{
				return false;
			}

			// Acquire the mouse.
			result = myMouse->Acquire();
			if (FAILED(result) == true)
			{
				return false;
			}
		}
		return true;
	}
	void InputWrapper::Shutdown()
	{
		// Release the mouse.
		if (myMouse != NULL)
		{
			myMouse->Unacquire();
			myMouse->Release();
			myMouse = NULL;
		}

		// Release the keyboard.
		if (myKeyboard != NULL)
		{
			myKeyboard->Unacquire();
			myKeyboard->Release();
			myKeyboard = NULL;
		}

		// Release the main interface to direct input.
		if (myDirectInputInterface != NULL)
		{
			myDirectInputInterface->Release();
			myDirectInputInterface = NULL;
		}
	}
	bool InputWrapper::Update()
	{
		if (myIsInitialized == true)
		{
			bool result;

			// Read the current state of the keyboard.
			result = ReadKeyboard();
			if (result == false)
			{
				return false;
			}

			// Read the current state of the mouse.
			result = ReadMouse();
			if (result == false)
			{
				return false;
			}
		}
		return true;
	}

	int InputWrapper::GetMousePositionX() const 
	{
		POINT point;
		GetCursorPos(&point);
		return point.x;
	}
	int InputWrapper::GetMousePositionY() const 
	{
		POINT point;
		GetCursorPos(&point);
		return point.y;
	}
	int InputWrapper::GetMouseRelativePositionX() const
	{
		return myMouseState.lX;
	}
	int InputWrapper::GetMouseRelativePositionY() const
	{
		return myMouseState.lY;
	}

	void InputWrapper::SetMousePosition(const int aMouseX, const int aMouseY)
	{
		SetCursorPos(aMouseX, aMouseY);
	}

	bool InputWrapper::IsKeyboardKeyDown(eKeys aKey) const
	{
		if (KEYBOARDKEYDOWN(aKey, myKeyboardState))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	bool InputWrapper::IsKeyboardKeyPressed(eKeys aKey) const
	{
		if ((KEYBOARDKEYDOWN(aKey, myKeyboardState)) && (KEYBOARDKEYUP(aKey, myPreviousKeyboardState)))
		{
			return true;
		}
		return false;
	}
	bool InputWrapper::IsKeyboardKeyReleased(eKeys aKey) const
	{
		if ((KEYBOARDKEYDOWN(aKey, myPreviousKeyboardState)) && (KEYBOARDKEYUP(aKey, myKeyboardState)))
		{
			return true;
		}
		return false;
	}

	bool InputWrapper::IsMouseButtonDown(eMouseButtons aButton) const
	{
		if (MOUSEBUTTONDOWN(aButton, myMouseState))
		{
			return true;
		}
		else
		{
			return false;
		}
	}
	bool InputWrapper::IsMouseButtonPressed(eMouseButtons aButton) const
	{
		if ((MOUSEBUTTONDOWN(aButton, myMouseState)) && (MOUSEBUTTONUP(aButton, myPreviousMouseState)))
		{
			return true;
		}
		return false;
	}
	bool InputWrapper::IsMouseButtonReleased(eMouseButtons aButton) const
	{
		if ((MOUSEBUTTONDOWN(aButton, myPreviousMouseState)) && (MOUSEBUTTONUP(aButton, myMouseState)))
		{
			return true;
		}
		return false;
	}

	int InputWrapper::GetMouseWheelPos() const
	{
		return myMouseState.lZ / 120;
	}

	bool InputWrapper::GetKeysDown(GrowingArray<eKeys, unsigned int, false>& aKeysDown)
	{
		bool hasAddedKey = false;
		aKeysDown.RemoveAll();

		for (int i = 0; i < 256; ++i)
		{
			if (myKeyboardState[i] & 0x80)
			{
				aKeysDown.Add(static_cast<CU::eKeys>(i));
				hasAddedKey = true;
			}
		}

		return hasAddedKey;
	}

	bool InputWrapper::GetKeysPressed(GrowingArray<eKeys, unsigned int, false>& aKeysPressed)
	{
		bool hasAddedKey = false;
		aKeysPressed.RemoveAll();

		for (int i = 0; i < 256; ++i)
		{
			if ((myKeyboardState[i] & 0x80) && (myPreviousKeyboardState[i] ^ myKeyboardState[i]))
			{
				aKeysPressed.Add(static_cast<CU::eKeys>(i));
				hasAddedKey = true;
			}
		}

		return hasAddedKey;
	}

	bool InputWrapper::GetKeysReleased(GrowingArray<eKeys, unsigned int, false>& aKeysPressed)
	{
		bool hasAddedKey = false;
		aKeysPressed.RemoveAll();

		for (int i = 0; i < 256; ++i)
		{
			if ((myPreviousKeyboardState[i] & 0x80) && (myPreviousKeyboardState[i] ^ myKeyboardState[i]))
			{
				aKeysPressed.Add(static_cast<CU::eKeys>(i));
				hasAddedKey = true;
			}
		}

		return hasAddedKey;
	}

	std::vector<std::string> InputWrapper::GetWhichButtonsDown()
	{
		std::vector<std::string> returnStrings;
		for (int i = 0; i < 256; ++i)
		{
			if (myKeyboardState[i] & 0x80)
			{
				returnStrings.push_back(GetKeyFromIndex(i));
			}
		}
		return returnStrings;
	}
	std::string InputWrapper::GetKeyFromIndex(int aIndex)
	{
		switch (static_cast<eKeys>(aIndex))
		{
		case eKeys::ESCAPE:return"ESCAPE";	break;
		case eKeys::ONE:return"ONE";	break;
		case eKeys::TWO:return"TWO";	break;
		case eKeys::THREE:return"THREE";	break;
		case eKeys::FOUR:return"FOUR";	break;
		case eKeys::FIVE:return"FIVE";	break;
		case eKeys::SIX:return"SIX";	break;
		case eKeys::SEVEN:return"SEVEN";	break;
		case eKeys::EIGHT:return"EIGHT";	break;
		case eKeys::NINE:return"NINE";	break;
		case eKeys::ZERO:return"ZERO";	break;
		case eKeys::MINUS:return"MINUS";	break;
		case eKeys::EQUALS:return"EQUALS";	break;
		case eKeys::BACK:return"BACK";	break;
		case eKeys::TAB:return"TAB";	break;
		case eKeys::Q:return"Q";	break;
		case eKeys::W:return"W";	break;
		//case eKeys::E:return"E";	break;
		case eKeys::R:return"R";	break;
		case eKeys::T:return"T";	break;
		case eKeys::Y:return"Y";	break;
		case eKeys::U:return"U";	break;
		case eKeys::I:return"I";	break;
		case eKeys::O:return"O";	break;
		case eKeys::P:return"P";	break;
		case eKeys::LBRACKET:return"LBRACKET";	break;
		case eKeys::RBRACKET:return"RBRACKET";	break;
		case eKeys::RETURN:return"RETURN";	break;
		case eKeys::LCONTROL:return"LCONTROL";	break;
		case eKeys::A:return"A";	break;
		case eKeys::S:return"S";	break;
		case eKeys::D:return"D";	break;
		case eKeys::F:return"F";	break;
		case eKeys::G:return"G";	break;
		case eKeys::H:return"H";	break;
		case eKeys::J:return"J";	break;
		case eKeys::K:return"K";	break;
		case eKeys::L:return"L";	break;
		case eKeys::SEMICOLON:return"SEMICOLON";	break;
		case eKeys::APOSTROPHE:return"APOSTROPHE";	break;
		case eKeys::GRAVE:return"GRAVE";	break;
		case eKeys::LSHIFT:return"LSHIFT";	break;
		case eKeys::BACKSLASH:return"BACKSLASH";	break;
		case eKeys::Z:return"Z";	break;
		case eKeys::X:return"X";	break;
		case eKeys::C:return"C";	break;
		case eKeys::V:return"V";	break;
		case eKeys::B:return"B";	break;
		case eKeys::N:return"N";	break;
		case eKeys::M:return"M";	break;
		case eKeys::COMMA:return"COMMA";	break;
		case eKeys::PERIOD:return"PERIOD";	break;
		case eKeys::SLASH:return"SLASH";	break;
		case eKeys::RSHIFT:return"RSHIFT";	break;
		case eKeys::MULTIPLY:return"MULTIPLY";	break;
		case eKeys::LMENU:return"LMENU";	break;
		case eKeys::SPACE:return"SPACE";	break;
		case eKeys::CAPITAL:return"CAPITAL";	break;
		case eKeys::F1:return"F1";	break;
		case eKeys::F2:return"F2";	break;
		case eKeys::F3:return"F3";	break;
		case eKeys::F4:return"F4";	break;
		case eKeys::F5:return"F5";	break;
		case eKeys::F6:return"F6";	break;
		case eKeys::F7:return"F7";	break;
		case eKeys::F8:return"F8";	break;
		case eKeys::F9:return"F9";	break;
		case eKeys::F10:return"F10";	break;
		case eKeys::NUMLOCK:return"NUMLOCK";	break;
		case eKeys::SCROLL:return"SCROLL";	break;
		case eKeys::NUMPAD7:return"NUMPAD7";	break;
		case eKeys::NUMPAD8:return"NUMPAD8";	break;
		case eKeys::NUMPAD9:return"NUMPAD9";	break;
		case eKeys::SUBTRACT:return"SUBTRACT";	break;
		case eKeys::NUMPAD4:return"NUMPAD4";	break;
		case eKeys::NUMPAD5:return"NUMPAD5";	break;
		case eKeys::NUMPAD6:return"NUMPAD6";	break;
		case eKeys::ADD:return"ADD";	break;
		case eKeys::NUMPAD1:return"NUMPAD1";	break;
		case eKeys::NUMPAD2:return"NUMPAD2";	break;
		case eKeys::NUMPAD3:return"NUMPAD3";	break;
		case eKeys::NUMPAD0:return"NUMPAD0";	break;
		case eKeys::DECIMAL:return"DECIMAL";	break;
		case eKeys::OEM_102:return"OEM_102";	break;
		case eKeys::F11:return"F11";	break;
		case eKeys::F12:return"F12";	break;
		case eKeys::F13:return"F13";	break;
		case eKeys::F14:return"F14";	break;
		case eKeys::F15:return"F15";	break;
		case eKeys::KANA:return"KANA";	break;
		case eKeys::ABNT_C1:return"ABNT_C1";	break;
		case eKeys::CONVERT:return"CONVERT";	break;
		case eKeys::NOCONVERT:return"NOCONVERT";	break;
		case eKeys::YEN:return"YEN";	break;
		case eKeys::ABNT_C2:return"ABNT_C2";	break;
		case eKeys::NUMPADEQUALS:return"NUMPADEQUALS";	break;
		case eKeys::PREVTRACK:return"PREVTRACK";	break;
		case eKeys::AT:return"AT";	break;
		case eKeys::COLON:return"COLON";	break;
		case eKeys::UNDERLINE:return"UNDERLINE";	break;
		case eKeys::KANJI:return"KANJI";	break;
		case eKeys::STOP:return"STOP";	break;
		case eKeys::AX:return"AX";	break;
		case eKeys::UNLABELED:return"UNLABELED";	break;
		case eKeys::NEXTTRACK:return"NEXTTRACK";	break;
		case eKeys::NUMPADENTER:return"NUMPADENTER";	break;
		case eKeys::RCONTROL:return"RCONTROL";	break;
		case eKeys::MUTE:return"MUTE";	break;
		case eKeys::CALCULATOR:return"CALCULATOR";	break;
		case eKeys::PLAYPAUSE:return"PLAYPAUSE";	break;
		case eKeys::MEDIASTOP:return"MEDIASTOP";	break;
		case eKeys::VOLUMEDOWN:return"VOLUMEDOWN";	break;
		case eKeys::VOLUMEUP:return"VOLUMEUP";	break;
		case eKeys::WEBHOME:return"WEBHOME";	break;
		case eKeys::NUMPADCOMMA:return"NUMPADCOMMA";	break;
		case eKeys::DIVIDE:return"DIVIDE";	break;
		case eKeys::SYSRQ:return"SYSRQ";	break;
		case eKeys::RMENU:return"RMENU";	break;
		case eKeys::PAUSE:return"PAUSE";	break;
		case eKeys::HOME:return"HOME";	break;
		case eKeys::UP:return"UP";	break;
		case eKeys::PRIOR:return"PRIOR";	break;
		case eKeys::LEFT:return"LEFT";	break;
		case eKeys::RIGHT:return"RIGHT";	break;
		case eKeys::END:return"END";	break;
		case eKeys::DOWN:return"DOWN";	break;
		case eKeys::NEXT:return"NEXT";	break;
		case eKeys::INSERT:return"INSERT";	break;
		case eKeys::DEL:return"DEL";	break;
		case eKeys::LWIN:return"LWIN";	break;
		case eKeys::RWIN:return"RWIN";	break;
		case eKeys::APPS:return"APPS";	break;
		case eKeys::POWER:return"POWER";	break;
		case eKeys::SLEEP:return"SLEEP";	break;
		case eKeys::WAKE:return"WAKE";	break;
		case eKeys::WEBSEARCH:return"WEBSEARCH";	break;
		case eKeys::WEBFAVORITES:return"WEBFAVORITES";	break;
		case eKeys::WEBREFRESH:return"WEBREFRESH";	break;
		case eKeys::WEBSTOP:return"WEBSTOP";	break;
		case eKeys::WEBFORWARD:return"WEBFORWARD";	break;
		case eKeys::WEBBACK:return"WEBBACK";	break;
		case eKeys::MYCOMPUTER:return"MYCOMPUTER";	break;
		case eKeys::MAIL:return"MAIL";	break;
		case eKeys::MEDIASELECT:return"MEDIASELECT";	break;
		default:
			break;
		}
		return "";
	}

	bool InputWrapper::ReadKeyboard()
	{
		HRESULT result;
		for (int i = 0; i < 256; ++i)
		{
			myPreviousKeyboardState[i] = myKeyboardState[i];
		}
		// Read the keyboard device.
		result = myKeyboard->GetDeviceState(sizeof(myKeyboardState), (LPVOID)&myKeyboardState);

		if (FAILED(result) == true)
		{
			// If the keyboard lost focus or was not acquired then try to get control back.
			if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
			{
				myKeyboard->Acquire();
			}
			else
			{
				return false;
			}
		}
		return true;	
	}
	bool InputWrapper::ReadMouse()
	{
		if (myIsInitialized == true)
		{
			HRESULT result;

			myPreviousMouseState = myMouseState;
			// Read the mouse device.
			result = myMouse->GetDeviceState(sizeof(DIMOUSESTATE), (LPVOID)&myMouseState);


			if (FAILED(result) == true)
			{
				// If the mouse lost focus or was not acquired then try to get control back.
				if ((result == DIERR_INPUTLOST) || (result == DIERR_NOTACQUIRED))
				{
					result = myMouse->Acquire();
				}
				else
				{
					return false;
				}
			}
		}

		return true;
	}
}
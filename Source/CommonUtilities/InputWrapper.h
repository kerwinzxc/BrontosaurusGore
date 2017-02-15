#pragma once
#pragma comment (lib, "dinput8.lib")
#pragma comment (lib, "dxguid.lib")
#define DIRECTINPUT_VERSION 0x0800
#include <dinput.h>
#include <string>
#include <vector>


namespace CU
{
	enum class eKeys;
	enum class eMouseButtons;
	
	template <typename ObjectType, typename SizeType = unsigned int, bool USE_SAFE_MODE = true>
	class GrowingArray;

	class InputWrapper
	{
	public:
		InputWrapper();
		~InputWrapper();

		bool Init(HINSTANCE hinstance, HWND hwnd);
		void Shutdown();
		bool Update();
		int GetMousePositionX() const;
		int GetMousePositionY() const;
		int GetMouseRelativePositionX() const;
		int GetMouseRelativePositionY() const;
		void SetMousePosition(const int aMouseX, const int aMouseY);
		bool IsKeyboardKeyDown(eKeys aKey) const;
		bool IsKeyboardKeyPressed(eKeys aKey) const;
		bool IsKeyboardKeyReleased(eKeys aKey) const;

		bool IsMouseButtonDown(eMouseButtons aButton) const;
		bool IsMouseButtonPressed(eMouseButtons aButton) const;
		bool IsMouseButtonReleased(eMouseButtons aButton) const;

		int GetMouseWheelPos() const;
		bool GetKeysDown(GrowingArray<eKeys, unsigned int, false>& aKeysDown);
		bool GetKeysPressed(GrowingArray<eKeys, unsigned int, false>& aKeysPressed);
		bool GetKeysReleased(GrowingArray<eKeys, unsigned int, false>& aKeysPressed);

		std::vector<std::string> GetWhichButtonsDown();// for testing all keys
	private:	
		std::string GetKeyFromIndex(int aIndex); // for testing all keys
		bool ReadKeyboard();
		bool ReadMouse();

		IDirectInput8* myDirectInputInterface;
		IDirectInputDevice8* myKeyboard;
		IDirectInputDevice8* myMouse;

		unsigned char myKeyboardState[256];
		unsigned char myPreviousKeyboardState[256];
		DIMOUSESTATE myMouseState;
		DIMOUSESTATE myPreviousMouseState;

		bool myIsInitialized;
	};
}  
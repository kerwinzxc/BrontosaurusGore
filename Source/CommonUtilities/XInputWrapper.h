#pragma once

struct _XINPUT_STATE;
typedef _XINPUT_STATE XINPUT_STATE;

namespace CU
{
	template <typename T>
	class Vector2;
	using Vector2f = Vector2<float>;

	class XInputWrapper
	{
	public:
		struct KeyStroke
		{
			enum class eKeyState : unsigned short
			{
				ePressed,
				eReleased,
				eRepeat

			} myKeyState;

			unsigned short myKeyCode;
		};

		XInputWrapper();
		~XInputWrapper();

		void Init(const unsigned int aJoystickCount);

		void UpdateStates();

		bool GetKeyStroke(const unsigned int aJoystickIndex, KeyStroke& aKeyStrokeOutput);
		bool GetKeyPressed(const unsigned int aJoystickIndex, const unsigned short aButton);
		bool GetKeyReleased(const unsigned int aJoystickIndex, const unsigned short aButton);

		bool IsConnected(const unsigned int aJoystickIndex, unsigned int* aError = nullptr);

		CU::Vector2f GetRightStickPosition(const unsigned int aJoystickIndex);
		CU::Vector2f GetLeftStickPosition(const unsigned int aJoystickIndex);
		bool LeftStickIsInDeadzone(const unsigned int aJoystickIndex);
		bool RightStickIsInDeadzone(const unsigned int aJoystickIndex);

		float GetLeftTriggerDown(const unsigned int aJoystickIndex);
		float GetRightTringgerDown(const unsigned int aJoystickIndex);

		void SetLeftVibration(const unsigned int aJoystickIndex, const unsigned short aAmount);
		void SetRightVibration(const unsigned int aJoystickIndex, const unsigned short aAmount);
		void SetFullControllerVibration(const unsigned int aJoystickIndex, const unsigned short aAmount);
		void StopVibration(const unsigned int aJoystickIndex);

		unsigned int GetConnectedJoystickCount() const;

	private:
		bool UpdateState(const unsigned int aJoystickIndex);
		void MoveDisconnectedJoysticks();

		GrowingArray<XINPUT_STATE*> myJoysticks;
		GrowingArray<XINPUT_STATE*> myDisconnectedJoysticks;


	public: //button constants
		const struct GAMEPAD
		{
			static const unsigned short DPAD_UP;
			static const unsigned short DPAD_DOWN;
			static const unsigned short DPAD_LEFT;
			static const unsigned short DPAD_RIGHT;
			static const unsigned short START;
			static const unsigned short BACK;
			static const unsigned short LEFT_THUMB;
			static const unsigned short RIGHT_THUMB;
			static const unsigned short LEFT_SHOULDER;
			static const unsigned short RIGHT_SHOULDER;
			static const unsigned short A;
			static const unsigned short B;
			static const unsigned short X;
			static const unsigned short Y;
		};
	};
}

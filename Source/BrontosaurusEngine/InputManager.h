#pragma once

#include "../PostMaster/Subscriber.h"

namespace CU
{
	class InputWrapper;
	class XInputWrapper;

	template <typename T>
	class Vector2;
	using Vector2f = Vector2<float>;

	enum class eKeys;
}


class CInputManager : public Subscriber
{
public:
	CInputManager();
	~CInputManager();

	void Update();
	eMessageReturn Recieve(const Message & aMessage) override;
	void SetMousePosition(const CU::Vector2f& aMousePosition);
	void LockUnlockMouse(const bool aHasFocus);
private:
	void UpdateMouse();
	void UpdateKeyboard();
	void UpdateGamePad();

	CU::GrowingArray<CU::eKeys, unsigned int, false> myKeyDowns;
	CU::Vector2f myLastMousePosition;
	CU::InputWrapper* myDInputWrapper;
	CU::XInputWrapper* myXInputWrapper;

	bool myHasFocus;
};

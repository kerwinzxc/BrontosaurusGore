#pragma once

#include "../PostMaster/Subscriber.h"

namespace CU
{
	class InputWrapper;
	class XInputWrapper;

	class CInputMessenger;

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

	void Listen(CU::CInputMessenger& aMessenger, const int aPriority);
	void Neglect(CU::CInputMessenger& aMessenger);
	static CInputManager* GetInstance();

private:
	void UpdateMouse();
	void UpdateKeyboard();
	void UpdateGamePad();

	CU::GrowingArray<CU::eKeys> myKeys;
	CU::GrowingArray<CU::CInputMessenger*> myMessengers;

	CU::Vector2f myLastMousePosition;
	CU::InputWrapper* myDInputWrapper;
	CU::XInputWrapper* myXInputWrapper;

	bool myHasFocus;

	static CInputManager* ourInstance;
};

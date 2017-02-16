#pragma once

namespace CU
{
	struct SInputMessage;
	enum class eInputReturn;

	class IInputListener
	{
	public:
		IInputListener();
		~IInputListener();

		virtual eInputReturn TakeInput(const SInputMessage& aInputMessage) = 0;

		void SetPriority(const int aPriority);
		int GetPriority() const;

	private:
		int myPriority;
	};
}

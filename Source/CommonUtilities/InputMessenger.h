#pragma once
#include "EInputMessengerType.h"

namespace CU
{
	struct SInputMessage;
	class IInputListener;
	enum class eInputReturn;

	class CInputMessenger
	{
	public:
		CInputMessenger(const eInputMessengerType aMessengerType, const int aPriority = 0);
		~CInputMessenger();

		void Subscribe(IInputListener& aListener);
		void Subscribe(IInputListener& aListener, const int aPriority);
		void Unsubscribe(IInputListener& aListener);

		virtual eInputReturn RecieveInput(const SInputMessage& aInputMessage);

		int GetPriority() const;

		static CInputMessenger* GetInstance(const eInputMessengerType aMessengerType);

	private:
		CU::GrowingArray<IInputListener*> myListeners;
		eInputMessengerType myMessengerType;
		int myPriority;

		static CU::StaticArray<CInputMessenger*, static_cast<int>(eInputMessengerType::eLength)> ourInstances;
	};
}

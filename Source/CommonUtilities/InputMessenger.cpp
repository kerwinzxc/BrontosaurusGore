#include "stdafx.h"
#include "InputMessenger.h"
#include "InputListener.h"
#include "EInputReturn.h"
#include "../BrontosaurusEngine/InputManager.h"

namespace CU
{
	CU::StaticArray<CInputMessenger*, static_cast<int>(eInputMessengerType::eLength)> CInputMessenger::ourInstances = nullptr;

	CInputMessenger::CInputMessenger(const eInputMessengerType aMessengerType, const int aPriority)
		: myListeners(4u)
		, myMessengerType(aMessengerType)
		, myPriority(aPriority)
	{
		CInputManager* inputManager = CInputManager::GetInstance();
		assert(inputManager != nullptr && "creating input messenger before input manager, maybe move to a init function?");
		if (inputManager != nullptr)
		{
			inputManager->Listen(*this, myPriority);
			ourInstances[static_cast<int>(myMessengerType)] = (this);
		}
	}

	CInputMessenger::~CInputMessenger()
	{
		CInputManager* inputManager = CInputManager::GetInstance();

		if (inputManager != nullptr)
		{
			inputManager->Neglect(*this);
		}

		//assert(ourInstances[static_cast<int>(myMessengerType)] == this && "Forgot to set the instance?");
		ourInstances[static_cast<int>(myMessengerType)] = nullptr;
	}

	void CInputMessenger::Subscribe(IInputListener& aListener)
	{
		myListeners.Add(&aListener);
	}

	void CInputMessenger::Subscribe(IInputListener& aListener, const int aPriority)
	{
		aListener.SetPriority(aPriority);

		for (unsigned int i = 0; i < myListeners.Size(); ++i)
		{
			if (myListeners[i]->GetPriority() < aPriority)
			{
				myListeners.Insert(i, &aListener);
				return;
			}
		}

		myListeners.Add(&aListener);
	}

	void CInputMessenger::Unsubscribe(IInputListener& aListener)
	{
		myListeners.Remove(&aListener);
	}

	eInputReturn CInputMessenger::RecieveInput(const SInputMessage& aInputMessage)
	{
		for (IInputListener* listener : myListeners)
		{
			if (listener->TakeInput(aInputMessage) == eInputReturn::eKeepSecret)
			{
				return eInputReturn::eKeepSecret;
			}
		}

		return eInputReturn::ePassOn;
	}

	int CInputMessenger::GetPriority() const
	{
		return myPriority;
	}

	CInputMessenger* CInputMessenger::GetInstance(const eInputMessengerType aMessengerType)
	{
		return ourInstances[static_cast<int>(aMessengerType)];
	}
}

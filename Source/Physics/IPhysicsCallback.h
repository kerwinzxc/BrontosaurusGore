#pragma once

namespace Physics
{
	class CPhysicsCallbackActor;

	class IPhysicsCallback
	{
	public:
		IPhysicsCallback();
		~IPhysicsCallback();

		virtual void OnTriggerEnter(CPhysicsCallbackActor* aOther) = 0;
		virtual void OnTriggerExit(CPhysicsCallbackActor* aOther) = 0;

		virtual void OnCollisionEnter(CPhysicsCallbackActor* aOther) = 0;
		virtual void OnCollisionExit(CPhysicsCallbackActor* aOther) = 0;

		inline void SetUserData(void* aDataPointer);
		inline void* GetUserData();

	private:
		void* myUserData;

	};
}

inline void Physics::IPhysicsCallback::SetUserData(void * aDataPointer)
{
	myUserData = aDataPointer;
}

inline void * Physics::IPhysicsCallback::GetUserData()
{
	return myUserData;
}

#pragma once

namespace Physics
{
	class IPhysicsCallback
	{
	public:
		IPhysicsCallback();
		~IPhysicsCallback();

		virtual void OnTriggerEnter(const IPhysicsCallback* aOther) = 0;
		virtual void OnTriggerExit(const IPhysicsCallback* aOther) = 0;

		virtual void OnCollisionEnter(const IPhysicsCallback* aOther) = 0;
		virtual void OnCollisionExit(const IPhysicsCallback* aOther) = 0;

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

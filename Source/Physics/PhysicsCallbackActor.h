#pragma once
namespace Physics
{
	class IPhysicsCallback;

	class CPhysicsCallbackActor
	{
	public:
		CPhysicsCallbackActor() { myCallback = nullptr; };
		~CPhysicsCallbackActor() { myCallback = nullptr; };

		virtual IPhysicsCallback* GetCallbackData() = 0;
		virtual void SetCallbackData(IPhysicsCallback* aCallbacker) = 0;

	protected:
		IPhysicsCallback* myCallback;
	};
}


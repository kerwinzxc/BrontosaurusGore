#pragma once
namespace Physics
{
	enum class EActorType
	{
		eStatic,
		eDynamic,
		eController
	};

	class IPhysicsCallback;

	class CPhysicsCallbackActor
	{
	public:
		CPhysicsCallbackActor() { myCallback = nullptr; };
		virtual ~CPhysicsCallbackActor() { myCallback = nullptr; };

		virtual IPhysicsCallback* GetCallbackData() = 0;
		virtual void SetCallbackData(IPhysicsCallback* aCallbacker) = 0;

		virtual const EActorType GetType() const = 0;
	protected:
		IPhysicsCallback* myCallback;
	};
}


#pragma once

typedef unsigned int ComponentId;
#define NULL_COMPONENT 0

class CGameObject;
struct SComponentMessageData;
enum class eComponentMessageType;

enum class eComponentType : unsigned char
{
	eGameObject,
	eModel,
	eModelDebug,
	eDebugSphere,
	eCamera,
	eInventory,
	eStat,
	eMainStat,
	eHealthBar,
	eHealth,
	eMana,
	ePlayerHealthMessenger,
	ePlayerManaMessenger,
	eStreakEmitter,
	eParticleEmitter,
	eSkillSystem,
	eSkill,
	eRespawn,
	eOpenShopListener,
	eMouse,
	eDrop,
	ePickup,
	ePickerUpper,
	eChangeLevelTrigger,
	eEnemyObjective,
	eActivationObjective,
	eNavigation,
	eMovement,
	eControllerInterface,
	eInputController,
	eAIController,
	eFleeController,
	eSeekController,
	eWanderController,
	eCollision,
	eAudioSource,
	eActivationComponent,
	ePointLight,
	eNone,
};

class CComponent
{
public:
	friend class CComponentManager;
	friend class CGameObject;

	CComponent();
	virtual ~CComponent();
	
	const eComponentType GetType() const;
	void NotifyParent(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData);

	//override this if you will need to get notified from other components
	virtual void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData);
	virtual void Destroy() = 0;
	virtual bool IsGameObject();

	__forceinline CGameObject* GetParent();

	ComponentId GetId()const;

protected:
	eComponentType myType;

private:
	CGameObject* myParent;
	ComponentId myId;
};

inline CGameObject* CComponent::GetParent()
{
	return myParent;
}

inline ComponentId CComponent::GetId() const
{
	return myId;
}

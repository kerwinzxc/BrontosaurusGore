#pragma once

typedef unsigned int ComponentId;
#define NULL_COMPONENT 0

class CGameObject;

struct SComponentMessageData;
enum class eComponentMessageType;

struct SComponentQuestionData;
enum class eComponentQuestionType;

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
	eNetwork,
	eNone,
};

class CComponent
{
public:
	friend class CComponentManager;
	friend class CGameObject;

	CComponent();
	virtual ~CComponent();
	
	void NotifyParent(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData);

	virtual void Receive(const eComponentMessageType aMessageType, const SComponentMessageData& aMessageData);
	virtual bool Answer(const eComponentQuestionType aQuestionType, SComponentQuestionData& aQuestionData);
	virtual void Destroy() {};
	virtual bool IsGameObject();

	__forceinline CGameObject* GetParent();
	__forceinline ComponentId GetId() const;
	__forceinline eComponentType GetType() const;

protected:
	eComponentType myType;

private:
	CGameObject* myParent;
	ComponentId myId;
};

__forceinline CGameObject* CComponent::GetParent()
{
	return myParent;
}

__forceinline ComponentId CComponent::GetId() const
{
	return myId;
}

__forceinline eComponentType CComponent::GetType() const
{
	return myType;
}

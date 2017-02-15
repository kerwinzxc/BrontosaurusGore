#pragma once
#include "../StateStack/State.h"
#include "../PostMaster/Subscriber.h"
#include "GameEventMessenger.h"
#include <atomic>



namespace CU
{
	class Time;
}

class CCollisionComponentManager;
class CGameObjectManager;
class CScene;


class CPlayState : public State , public Subscriber
{
public:
	CPlayState(StateStack& aStateStack, const int aLevelIndex, const bool aShouldReturnToLevelSelect = false);
	~CPlayState();
	void Load();

	void Init() override;
	eStateStatus Update(const CU::Time& aDeltaTime) override;
	void Render() override;
	void OnEnter(const bool aLetThroughRender) override;
	void OnExit(const bool aLetThroughRender) override;
	void Pause();
	void CheckReturnToLevelSelect();

	eMessageReturn Recieve(const Message & aMessage);

	CGameObjectManager* GetObjectManager();
	CCollisionComponentManager* GetCollisionManager();

	inline bool IsLoaded() const;

private:
	void CreateManagersAndFactories();

private:
	CScene* myScene;
	int myLevelIndex;
	std::atomic_bool  myIsLoaded;
	bool myShouldReturnToLevelSelect;
};

inline bool CPlayState::IsLoaded() const
{
	return myIsLoaded;
}


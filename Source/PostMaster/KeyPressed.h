#pragma once
#include "Event.h"

namespace CU
{
	enum class eKeys;
}

class KeyPressed : public Event
{
public:
	KeyPressed(const CU::eKeys& aKey);
	~KeyPressed();

	eMessageReturn DoEvent(CDebugInfoDrawer* aDebugInfoDrawer) const override;
	eMessageReturn DoEvent(CreditsState*) const override;
	eMessageReturn DoEvent(CPlayState*) const override;
	eMessageReturn DoEvent(CSplashScreen* aSplashScreen) const override;
	eMessageReturn DoEvent(PauseMenu*) const override;
	eMessageReturn DoEvent(CConsole* aConsole) const override;
	eMessageReturn DoEvent(CCollisionComponentManager* aHatShop) const override;

	//temp
	eMessageReturn DoEvent(CRenderer* aRenderer) const override;

private:
	const CU::eKeys& myKey;
};


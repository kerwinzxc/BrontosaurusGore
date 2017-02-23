#pragma once

#include "EMessageReturn.h"

namespace GUI
{
	class GUIManager;
}


class State;
class PauseMenu;
class GUICursor;
class CPlayState;
class StateStack;
class CreditsState;
class CInputManager;
class CSplashScreen;
class CDebugInfoDrawer;
class LevelSelectState;
class CConsole;
class CMouseComponent;
class CModelViewer;
class CCollisionComponentManager;
class CGameEventMessenger;
class CClient;

class CRenderer;

class Event
{
public:
	Event() {}
	virtual ~Event() {}

	virtual eMessageReturn DoEvent(State*) const { return eMessageReturn::eContinue; }
	virtual eMessageReturn DoEvent(PauseMenu*) const { return eMessageReturn::eContinue; }
	virtual eMessageReturn DoEvent(GUICursor*) const { return eMessageReturn::eContinue; }
	virtual eMessageReturn DoEvent(CPlayState*) const { return eMessageReturn::eContinue; }
	virtual eMessageReturn DoEvent(StateStack*) const { return eMessageReturn::eContinue; }
	virtual eMessageReturn DoEvent(CreditsState*) const { return eMessageReturn::eContinue; }
	virtual eMessageReturn DoEvent(CSplashScreen*) const { return eMessageReturn::eContinue; }
	virtual eMessageReturn DoEvent(CInputManager*) const { return eMessageReturn::eContinue; }
	virtual eMessageReturn DoEvent(GUI::GUIManager*) const { return eMessageReturn::eContinue; }
	virtual eMessageReturn DoEvent(CDebugInfoDrawer*) const { return eMessageReturn::eContinue; }
	virtual eMessageReturn DoEvent(LevelSelectState*) const { return eMessageReturn::eContinue; }
	virtual eMessageReturn DoEvent(CConsole*) const { return eMessageReturn::eContinue; }
	virtual eMessageReturn DoEvent(CMouseComponent*) const { return eMessageReturn::eContinue; }
	virtual eMessageReturn DoEvent(CModelViewer*) const { return  eMessageReturn::eContinue; }
	virtual eMessageReturn DoEvent(CCollisionComponentManager*) const { return  eMessageReturn::eContinue; }
	virtual eMessageReturn DoEvent(CClient*) const { return eMessageReturn::eContinue; }
	
	virtual eMessageReturn DoEvent(CGameEventMessenger*) const { return eMessageReturn::eContinue; }

	//temp
	virtual eMessageReturn DoEvent(CRenderer*) const { return eMessageReturn::eContinue; }
};

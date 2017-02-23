#include "../StateStack/StateStack.h"
#include "BackgroundLoadingManager.h"
#include "../TClient/Client.h"

namespace CU
{
	class Time;
}

class CSplashScreen;

class CGame
{
public:
	CGame();
	~CGame();

	void Init();
	void Update(const CU::Time& aDeltaTime);
	void Render();

private:
	StateStack myStateStack;
	CSplashScreen* mySplashScreen;
	CClient myClient;

};

#include "stdafx.h"

#include "../CommonUtilities/DL_Debug.h"
#include "../PostMaster/PostMaster.h"
#include "../BrontosaurusEngine/EDebugInfoDrawerFlags.h"
#include "../Audio/AudioInterface.h"
#include "../Game/Game.h"
#include "../BrontosaurusEngine/Engine.h"
#include "../ThreadedPostmaster/Postmaster.h"
#include "../Physics/PhysXManager.h"

unsigned int DebugDrawerFlags()
{
#ifdef _DEBUG
	return eDebugFlags_FPS | eDebugFlags_LogicFPS | eDebugFlags_DrawCalls | eDebugFlags_MemoryUsage;
#else //NDEBUG || _RETAIL_BUILD
	return 0;
#endif // _DEBUG
}

void InitPhysics()
{
	Physics::CPhysXManager::Create();
}

void Update(const CU::Time& aDt)
{
	int i = 0;
}

void Render()
{
	int i = 0;
}

void InitEngine()
{
	SInitEngineParams engineParams;
	engineParams.myWindowParams.Width = 1920;
	engineParams.myWindowParams.Height = 1080;
	engineParams.myWindowParams.Name = L"Hat 'n' Slash";
	engineParams.myWindowParams.Title = L"Hat 'n' Slash";
	engineParams.myWindowParams.Fullscreen = false;
	engineParams.myThreadRender = true;
	engineParams.myUseVsync = false;

	int Iwidth;
	int Iheight;
	bool Bfullscreen = true;

	engineParams.myInitCallbackFunction = std::bind(&InitPhysics);
	//engineParams.myUpdateCallbackFunction = std::bind(&Update,std::placeholders::_1);
	engineParams.myRenderCallbackFunction = std::bind(&Render);
	engineParams.myDebugFlags = DebugDrawerFlags();
	
	CEngine::GetInstance()->Init(engineParams);
	CEngine::GetInstance()->Start();
}



int main(int argc, char* argv[])
{
	Postmaster::Threaded::CPostmaster::Create();
	DL_Debug::Debug::CreateInstance();

	InitEngine();

	Physics::CPhysXManager::Destroy();
	CEngine::DestroyInstance();
	PostMaster::DestroyInstance();
	DL_Debug::Debug::DestroyInstance();

	Postmaster::Threaded::CPostmaster::Destroy();

	return 0;
}
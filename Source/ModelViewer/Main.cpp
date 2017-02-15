#include "stdafx.h"
#include "ModelViewer.h"

int main(int argc, char* argv[])
{
	CommandLineManager::CreateInstance(argc, argv);
	DL_Debug::Debug::CreateInstance();
	PostMaster::CreateInstance();

	{
		CModelViewer modelViewer;
		SInitEngineParams engineParams;
		engineParams.myWindowParams.Width = 1920;
		engineParams.myWindowParams.Height = 1080;
		engineParams.myWindowParams.Name = L"Brontosaurus-Fez-ModelViewer";
		engineParams.myWindowParams.Title = L"Brontosaurus-Fez-ModelViewer";
		engineParams.myWindowParams.Fullscreen = false;
		engineParams.myThreadRender = false;

		const char* modelPath = (argc >= 2) ? argv[1] : nullptr;
		engineParams.myInitCallbackFunction = std::bind(&CModelViewer::Init, &modelViewer, modelPath);
		engineParams.myUpdateCallbackFunction = std::bind(&CModelViewer::Update, &modelViewer, std::placeholders::_1);
		engineParams.myRenderCallbackFunction = std::bind(&CModelViewer::Render, &modelViewer);

		engineParams.myDebugFlags = eDebugFlags_FPS | eDebugFlags_LogicFPS | eDebugFlags_DrawCalls | eDebugFlags_MemoryUsage;

		CEngine::GetInstance()->Init(engineParams);
		CEngine::GetInstance()->Start();
	}

	CEngine::DestroyInstance();
	PostMaster::DestroyInstance();
	DL_Debug::Debug::DestroyInstance();
	CommandLineManager::DestroyInstance();

	DumpMemoryLeeks();
	return 0;
}

#pragma once

class CLoaderScene;
class CLoaderCamera;

namespace CU
{
	class Camera;
	class DynamicString;
}

namespace GUI
{
	class IWidget;
	class WidgetContainer;

	class WidgetFactory
	{
	public:
		static WidgetContainer* CreateGUIScene(const char* aFilePathFBX, CU::Camera*& aGUIManagerCameraOut);

		static const CU::Camera* GetCurrentGUICamera();

		static std::map<std::string, IWidget*> ourHatContainer;

	private:
		static WidgetContainer* CreateGUIScene(const CLoaderScene* aLoaderScene, CU::Camera*& aGUIManagerCameraOut, const std::string& aJsonPath);
		static IWidget* CreateButton(IWidget* aWidget);		

		static CU::Camera* ParseCamera(const CLoaderCamera* aCamera);

		static const CU::Camera* ourCurrentGUICamera;
	};
}

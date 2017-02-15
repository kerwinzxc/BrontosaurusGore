#pragma once
#include "Widget.h"

class ModelComponent;
class CGameObject;

namespace GUI
{
	class SpriteWidget : public IWidget
	{
	public:
		SpriteWidget(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const std::string& aName, ModelComponent* aModelComponent, const bool aIsVisible = true);
		~SpriteWidget();

		void Render() override;

	private:
		CGameObject* myRenderObject;
	};
}

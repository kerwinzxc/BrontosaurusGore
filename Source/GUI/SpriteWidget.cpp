#include "stdafx.h"
#include "SpriteWidget.h"

//#include "../Dx3DEngine/Model/ModelComponent.h"
//#include "../Dx3DEngine/Components/GameObject/GameObject.h"

namespace GUI
{
	SpriteWidget::SpriteWidget(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const std::string& aName, ModelComponent* /*aModelComponent*/, const bool aIsVisible)
		: IWidget(aPosition, aSize, aName, aIsVisible)
		, myRenderObject(nullptr)
	{
		//myRenderObject = new CGameObject();
		//myRenderObject->AddComponent(aModelComponent);

		//myRenderObject->GetTransformReference().SetPosition(CU::Vector4f(GetWorldPosition().x, GetWorldPosition().y, 0.f, 1.f));
	}

	SpriteWidget::~SpriteWidget()
	{
		//SAFE_DELETE(myRenderObject);
	}
	void SpriteWidget::Render()
	{
	}
}

#include "stdafx.h"
#include "ModelWidget.h"

#include "../BrontosaurusEngine/Engine.h"
#include "../BrontosaurusEngine/FBXLoader.h"

#include "../BrontosaurusEngine/Renderer.h"
#include "../BrontosaurusEngine/ModelManager.h"
#include "../BrontosaurusEngine/ModelInstance.h"

#include "../CommonUtilities/Camera.h"
#include "GUIPixelConstantBuffer.h"

const float MillisecondsToFlash = 500.f;

static const CU::Camera* locGUICamera = nullptr;

namespace GUI
{
	ModelWidget::ModelWidget(CLoaderMesh* aLoaderMesh, const CU::GrowingArray<std::string>& aTexturePaths, const CU::Camera& aGUICamera, const bool aIsVisible)
		: IWidget(CU::Vector2f::Zero, CU::Vector2f::One, aLoaderMesh->myName, aIsVisible)
		, myModelInstance(nullptr)
		, myPixelConstantBufferStruct(nullptr)
		, myMillisecondsLeftSinceMouseEnter(0.f)
	{
		locGUICamera = &aGUICamera;

		CModelManager* modelManager = MODELMGR;
		CModelManager::ModelId model = modelManager->LoadGUIModel(aLoaderMesh, aTexturePaths);
		myModelInstance = new CModelInstance(model, aLoaderMesh->myTransformation);
		myPixelConstantBufferStruct = new SPixelConstantBuffer();
		myOriginalTransformation = aLoaderMesh->myTransformation;
		myMinPoint = aLoaderMesh->myMinPoint;
		myMaxPoint = aLoaderMesh->myMaxPoint;

		CU::Vector2f screenMinPosition;
		ConvertPosition3DTo2D(aGUICamera, aLoaderMesh->myMinPoint, screenMinPosition);

		CU::Vector2f screenMaxPosition;
		ConvertPosition3DTo2D(aGUICamera, aLoaderMesh->myMaxPoint, screenMaxPosition);

		SetWorldPosition(CU::Vector2f(screenMinPosition.x, 1.f - screenMaxPosition.y));
		SetSize((screenMaxPosition - screenMinPosition));
		AddDebugLines();
	}

	ModelWidget::ModelWidget(CModelInstance* aModelInstance, const CU::Camera& aGUICamera, const std::string& aName, const bool aIsVisible)
		: IWidget(CU::Vector2f::Zero, CU::Vector2f::One, aName, aIsVisible)
		, myModelInstance(aModelInstance)
		, myPixelConstantBufferStruct(nullptr)
		, myMillisecondsLeftSinceMouseEnter(0.f)
	{
		assert(myModelInstance != nullptr && "got model instance that was null");

		myPixelConstantBufferStruct = new SPixelConstantBuffer();
		myOriginalTransformation = myModelInstance->GetTransformation();
		myMinPoint = myModelInstance->GetModelBoundingBox().myMinPos;
		myMaxPoint = myModelInstance->GetModelBoundingBox().myMaxPos;

		CU::Vector2f screenMinPosition;
		ConvertPosition3DTo2D(aGUICamera, myMinPoint, screenMinPosition);

		CU::Vector2f screenMaxPosition;
		ConvertPosition3DTo2D(aGUICamera, myMaxPoint, screenMaxPosition);

		SetWorldPosition(CU::Vector2f(screenMinPosition.x, 1.f - screenMaxPosition.y));
		SetSize((screenMaxPosition - screenMinPosition));
		AddDebugLines();
	}

	ModelWidget::~ModelWidget()
	{
		SAFE_DELETE(myModelInstance);
		SAFE_DELETE(myPixelConstantBufferStruct);
	}

	void ModelWidget::SetInactivePosition(const CU::Vector3f& aPosition)
	{
		if (aPosition == aPosition.Zero)
		{
			myModelInstance->SetTransformation(myOriginalTransformation);
		}
		else
		{
			CU::Matrix44f newTransformation(myOriginalTransformation);
			newTransformation.myPosition += aPosition;
			myModelInstance->SetTransformation(newTransformation);
		}
	}

	void ModelWidget::SetVisibility(const bool aVisibility)
	{
		IWidget::SetVisibility(aVisibility);
		myModelInstance->SetVisibility(aVisibility);
	}

	SPixelConstantBuffer& ModelWidget::GetPixelConstantBufferStruct()
	{
		assert(myPixelConstantBufferStruct != nullptr && "Pixel constant buffer struct is NULL in ModelWidet :-(");
		return *myPixelConstantBufferStruct;
	}

	void ModelWidget::Update(const CU::Time& aDeltaTime)
	{
		if (myMillisecondsLeftSinceMouseEnter > 0.f)
		{
			myMillisecondsLeftSinceMouseEnter = max(0.f, myMillisecondsLeftSinceMouseEnter - aDeltaTime.GetMilliseconds());

			myPixelConstantBufferStruct->myValues[SPixelConstantBuffer::eMember::eFlashButton] = myMillisecondsLeftSinceMouseEnter / MillisecondsToFlash;
		}
	}

	void ModelWidget::OnMouseEnter(const CU::Vector2f& aMousePosition)
	{
		SUPRESS_UNUSED_WARNING(aMousePosition);
	}

	void ModelWidget::OnPositionChanged(const CU::Vector2f aDisplacement)
	{
		if (locGUICamera == nullptr || GetSize() == CU::Vector2f::One)
		{
			return;
		}

		CU::Vector2f newPosition = GetLocalPosition();
		CU::Vector2f oldPosition = newPosition - aDisplacement;
		

		//convert pixel mouse position to world ground position
		CU::Vector2f mousePosNormalizedSpace = newPosition * 2.f - CU::Vector2f::One;
		mousePosNormalizedSpace.y *= -1;
		CU::Vector4f mousePosNormalizedHomogeneousSpace(mousePosNormalizedSpace, CU::Vector2f::Zero);
		CU::Vector4f screenToCameraSpaceRay = mousePosNormalizedHomogeneousSpace * locGUICamera->GetProjectionInverse();

		CU::Vector3f direction;
		direction.x = (screenToCameraSpaceRay.x * locGUICamera->GetTransformation().m11) + (screenToCameraSpaceRay.y * locGUICamera->GetTransformation().m21) + locGUICamera->GetTransformation().m31;
		direction.y = (screenToCameraSpaceRay.x * locGUICamera->GetTransformation().m12) + (screenToCameraSpaceRay.y * locGUICamera->GetTransformation().m22) + locGUICamera->GetTransformation().m32;
		direction.z = (screenToCameraSpaceRay.x * locGUICamera->GetTransformation().m13) + (screenToCameraSpaceRay.y * locGUICamera->GetTransformation().m23) + locGUICamera->GetTransformation().m33;

		CU::Vector3f targetPosition3D;
		const CU::Vector3f groundNormal(0.f, 1.f, 0.f);
		const float denominator = direction.Dot(groundNormal);
		if (std::fabs(denominator) > 0.0001f)
		{
			const float t = (myModelInstance->GetTransformation().GetPosition() - locGUICamera->GetPosition()).Dot(groundNormal) / denominator;
			if (std::fabs(t) > 0.0001f)
			{
				targetPosition3D = locGUICamera->GetPosition() + direction * t;
			}
		}
		CU::Vector3f modelPos = myModelInstance->GetTransformation().GetPosition();
		myModelInstance->SetPosition(targetPosition3D + CU::Vector3f(myMaxPoint.x - myMinPoint.x, 0.f, myMaxPoint.z - myMinPoint.z) - modelPos);
	}

	void ModelWidget::Render()
	{
		if (IsVisible() == true)
		{
			SRenderGUIModelMessage* renderMessage = new SRenderGUIModelMessage();
			if (renderMessage != nullptr)
			{
				renderMessage->myModelID = myModelInstance->GetModelID();
				renderMessage->myToWorld = myModelInstance->GetTransformation();
				renderMessage->myPixelConstantBufferStruct = *myPixelConstantBufferStruct;

				RENDERER.AddRenderMessage(renderMessage);
			}
		}
	}

	void ModelWidget::SetFlashTimeToMax()
	{
		myMillisecondsLeftSinceMouseEnter = MillisecondsToFlash;
	}

	void ModelWidget::ConvertPosition3DTo2D(const CU::Camera& aGUICamera, const CU::Vector3f& aPosition3D, CU::Vector2f& aPosition2D)
	{
		CU::Vector4f minPosition(aPosition3D);
		minPosition = minPosition * myModelInstance->GetTransformation();
		minPosition = minPosition * aGUICamera.GetInverse();
		minPosition = minPosition * aGUICamera.GetProjection();
		minPosition /= minPosition.w;
		CU::Vector2f screenMinPosition(minPosition.x, minPosition.y);
		screenMinPosition = (screenMinPosition + CU::Vector2f::One) / 2.f;
		aPosition2D = screenMinPosition;
	}
}

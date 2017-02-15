#include "stdafx.h"
#include "WidgetContainer.h"

namespace GUI
{
	WidgetContainer::WidgetContainer(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const bool aIsVisible)
		: IWidget(aPosition, aSize, aIsVisible)
		, myOrderedWidgets(4)
		, myFrontLayerWidgets(4)
	{
	}

	WidgetContainer::WidgetContainer(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const std::string& aName, const bool aIsVisible)
		: IWidget(aPosition, aSize, aName, aIsVisible)
		, myOrderedWidgets(4)
		, myFrontLayerWidgets(4)
	{
	}

	WidgetContainer::~WidgetContainer()
	{
		for (auto it = myWidgets.begin(); it != myWidgets.end(); ++it)
		{
			SAFE_DELETE(it->second);
		}

		myWidgets.clear();
	}

	void WidgetContainer::AddWidget(const std::string& aWidgetName, IWidget* aWidget)
	{
		assert(myWidgets.find(aWidgetName) == myWidgets.end() && "There is already a IWidget with that name.");
		myWidgets[aWidgetName] = aWidget;
		myOrderedWidgets.Insert(0, aWidget);
		SetChild(aWidget);
	}

	IWidget* WidgetContainer::RemoveWidget(const std::string& aWidgetName)
	{
		auto it = myWidgets.find(aWidgetName);
		if (it != myWidgets.end())
		{
			IWidget* removedWidget = it->second;
			myWidgets.erase(it);
			myOrderedWidgets.Remove(removedWidget);
			return removedWidget;
		}

		//for (auto it = myWidgets.begin(); it != myWidgets.end(); ++it)
		//{
		//	if (aWidgetName == it->first)
		//	{
		//		IWidget* removedWidget = it->second;
		//		myWidgets.erase(it);
		//		myOrderedWidgets.Remove(removedWidget);
		//		return removedWidget;
		//	}
		//}

		return nullptr;
	}

	IWidget* WidgetContainer::FindWidget(const std::string& aWidgetName)
	{
		auto it = myWidgets.find(aWidgetName);
		if (it != myWidgets.end())
		{
			return it->second;
		}
		else
		{
			for (it = myWidgets.begin(); it != myWidgets.end(); ++it)
			{
				IWidget* widget = it->second->FindWidget(aWidgetName);
				if (widget != nullptr)
				{
					return widget;
				}
			}
		}

		return nullptr;
	}

	void WidgetContainer::Update(const CU::Time& aDeltaTime)
	{
		for (auto it = myWidgets.begin(); it != myWidgets.end(); ++it)
		{
			it->second->Update(aDeltaTime);
		}
	}

	void WidgetContainer::Render()
	{
		if (IsVisible() == true)
		{
			//int ii = myOrderedWidgets.Size() - 1;
			//bool succeeded = true;
			//auto reversed = CU::MakeReversed(myOrderedWidgets);
			//for (IWidget* widget : reversed)
			//{
			//	widget->Render(myOrderedWidgets);
			//	IWidget* sameWidget = myOrderedWidgets[ii];
			//	if (widget != sameWidget)
			//	{
			//		succeeded = false;
			//	}
			//	--ii;
			//}


			//for (CU::GrowingArray<IWidget*>::iterator it = myOrderedWidgets.rbegin(); it != myOrderedWidgets.rend(); --it)
			//{
			//	IWidget* widget = *it;
			//	if (widget->IsVisible() == true)
			//	{
			//		widget->Render(myFrontLayerWidgets);
			//	}
			//}
			//for (int i = myOrderedWidgets.Size() - 1; i >= 0; --i)
			//{
			//	IWidget& child = *myOrderedWidgets[i];
			//	if (child.IsVisible() == true)
			//	{
			//		child.Render(myFrontLayerWidgets);
			//	}
			//}

			Render(myFrontLayerWidgets);
		}
	}

	void WidgetContainer::Render(CU::GrowingArray<IWidget*>& aWidgets)
	{
		for (int i = myOrderedWidgets.Size() - 1; i >= 0; --i)
		{
			IWidget& child = *myOrderedWidgets[i];
			if (child.IsVisible() == true)
			{
				child.Render(aWidgets);
			}
		}
	}

	void WidgetContainer::SetVisibilityRecursive(const bool aVisibility)
	{
		for (auto it = myWidgets.begin(); it != myWidgets.end(); ++it)
		{
			it->second->SetVisibility(aVisibility);
		}

		IWidget::SetVisibility(aVisibility);
	}

	void WidgetContainer::RemoveDebugLines() const
	{
		for (unsigned int i = 0; i < myOrderedWidgets.Size(); ++i)
		{
			if (myOrderedWidgets[i]->HasDebugLines())
			{
				myOrderedWidgets[i]->RemoveDebugLines();
			}
		}
	}

	IWidget* WidgetContainer::MouseIsOver(const CU::Vector2f& aPosition)
	{
		IWidget* container = IWidget::MouseIsOver(aPosition);
		if (container != nullptr)
		{
			CU::GrowingArray<IWidget*> hoveredWidgets(4u);
			for (IWidget* widget : myOrderedWidgets)
			//for (int i = myOrderedWidgets.Size() - 1; i >= 0; --i)
			{
				IWidget* selectedWidget = widget->MouseIsOver(aPosition);
				if (selectedWidget != nullptr)
				{
					hoveredWidgets.Add(selectedWidget);
					//return selectedWidget;
				}
			}

			if (!hoveredWidgets.Empty())
			{
				auto it = myWidgets.find("PlayerHealthWidget");
				if (it != myWidgets.end())
				{
					if (hoveredWidgets.Find(it->second) != hoveredWidgets.FoundNone)
					{
						return it->second;
					}
				}
				it = myWidgets.find("PlayerManaWidget");
				if (it != myWidgets.end())
				{
					if (hoveredWidgets.Find(it->second) != hoveredWidgets.FoundNone)
					{
						return it->second;
					}
				}
				IWidget* whirl = FindWidget("WhirlwindHat");
				if (whirl && hoveredWidgets.Find(whirl) != hoveredWidgets.FoundNone)
				{
					int br = 0;
				}
				for (IWidget* widg : hoveredWidgets)
				{
					DL_PRINT("%s", widg->GetName().c_str());
				}

				return hoveredWidgets.GetFirst();
			}
		}

		return container;
	}

	void WidgetContainer::RenderFrontLayers()
	{
		for (IWidget* widget : myFrontLayerWidgets)
		{
			widget->Render();
		}

		myFrontLayerWidgets.RemoveAll();
	}

	void WidgetContainer::MoveToFront(const std::string& aWidgetName)
	{
		auto it = myWidgets.find(aWidgetName);
		if (it != myWidgets.end())
		{
			IWidget* widgetToMove = it->second;
			myOrderedWidgets.Remove(it->second);
			myOrderedWidgets.Insert(0, widgetToMove);
		}
	}

	void WidgetContainer::MoveToBack(const std::string& aWidgetName)
	{
		auto it = myWidgets.find(aWidgetName);
		if (it != myWidgets.end())
		{
			IWidget* widgetToMove = it->second;
			myOrderedWidgets.Remove(it->second);
			myOrderedWidgets.Add(widgetToMove);
		}
		else
		{
			DL_PRINT("coudn't find %s", aWidgetName.c_str());
		}
	}
}

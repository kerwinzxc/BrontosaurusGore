#pragma once
#include "WidgetContainer.h"

namespace CU
{
	template <typename T>
	class Vector2;
	using Vector2f = Vector2<float>;
}

namespace GUI
{
	class Window : public WidgetContainer
	{
	public:
		Window(const CU::Vector2f& aPosition, const CU::Vector2f& aSize);
		Window(const CU::Vector2f& aPosition, const CU::Vector2f& aSize, const std::string& aName);
		~Window();

		void Render() override;
		void OnPositionChanged() override;

	private:
		void Init(const std::string& aBackgroundSpritePath, const CU::Vector2f& aBackgroundCoords);
	};
}

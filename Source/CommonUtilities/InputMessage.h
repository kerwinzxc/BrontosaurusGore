#pragma once

namespace CU
{
	enum class eMouseButton;
	enum class eKeys;

	enum class eInputType
	{
		eMouseMoved,
		eMousePressed,
		eMouseReleased,
		eKeyboardPressed,
		eKeyboardReleased
	};

	struct SInputMessage
	{
		SInputMessage() : myType(eInputType::eMouseMoved) {}
		union
		{
			Vector2f myMousePosition;
			Vector2f myMouseDelta;
		};

		union
		{
			eMouseButton myMouseButton;
			eKeys myKey;
		};

		eInputType myType;
	};
}

static_assert(sizeof(CU::SInputMessage) <= sizeof(CU::Vector2f) * 2, "Allignement is fucked, enums different sizes?");

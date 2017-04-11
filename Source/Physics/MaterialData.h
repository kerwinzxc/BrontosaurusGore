#pragma once;

namespace Physics
{
	const float defMatStaticFriction = 0.5f;
	const float defMatDynamicFriction = 0.5f;
	const float defMatRestitution = 0.5f;

	struct SMaterialData
	{
		float aStaticFriction = defMatStaticFriction;
		float aDynamicFriction = defMatDynamicFriction;
		float aRestitution = defMatRestitution;
	};
}
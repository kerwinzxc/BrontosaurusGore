#pragma once

class CModelComponent;
struct SAnimation;

namespace AnimationComponentLoader
{
	void LoadAnimations(const CModelComponent& aModelComponent, std::map<std::string, SAnimation>& aAnimationStates);
};

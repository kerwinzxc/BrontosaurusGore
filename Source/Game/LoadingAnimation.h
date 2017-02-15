#pragma once
#include "vector2.h"

namespace CU {
	class Time;
}

class CSpriteInstance;

class CLoadingAnimation
{
public:
	CLoadingAnimation();
	~CLoadingAnimation();

	void Init(CSpriteInstance* aSpriteInstance);
	void Update(const CU::Time &aDeltatime);
	void Render();

private:
	CU::Vector2f myPosition;
	CSpriteInstance* mySpriteInstance;
};


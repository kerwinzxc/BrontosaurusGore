#pragma once
#include "vector4.h"
#include "Colour.h"

enum class eBarInterMode
{
	eLinear,
	eCirkular
};

class CBarInstance
{
public:
	CBarInstance(const CU::Colour& aFullColour, const CU::Vector4f& aRect);
	CBarInstance(const CU::Colour& aFullColour, const CU::Colour& anEmptyColour, const CU::Vector4f& aRect, const eBarInterMode aInterMode = eBarInterMode::eLinear);

	~CBarInstance();

	void Update(CU::Time aDeltaTime);
	void Render() const;

	void SetLevel(const float aLevel);

private:
	float myCurrentLevel;
	
	const CU::Colour myFullColour;
	const CU::Colour myBackgroundColour;
	const CU::Vector4f myRect;

	struct SBarInterpolationData
	{
		CU::Colour myEmptyColour;
		eBarInterMode myInterMode;
	}* const myInterpolationData;
};


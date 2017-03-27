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
	CBarInstance(const CU::Colour& aFullColour, const CU::Colour& anEmptyColour, const CU::Vector4f& aRect);

	~CBarInstance();

	void Update(CU::Time aDeltaTime);
	void Render() const;
	void RenderToGUI(const std::wstring &aStr) const;

	float GetLevel() const;
	void SetLevel(const float aLevel);

private:
	float myCurrentLevel;
	
	const CU::Colour myFullColour;
	const CU::Colour myBackgroundColour;
	const CU::Vector4f myRect;
	const CU::Colour myEmptyColour;
};


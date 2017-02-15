#include "stdafx.h"
#include "Tween.h"

#include "dbtweener.h"

#define self (*this)

namespace CU
{
	Tween::Tween(TweenType aType, TweenMod aMod, float aStart, float aEnd, float aDuration)
		: myTweener(nullptr)
		, myValue(aStart)
		, myProgress(0.f)
		, myDuration(aDuration)
	{
		CDBTweener::CEquation *equation = nullptr;

		switch (aType)
		{
		case TweenType::Linear:
			equation = &CDBTweener::TWEQ_LINEAR;
			break;
		case TweenType::Sinusoidal:
			equation = &CDBTweener::TWEQ_SINUSOIDAL;
			break;
		case TweenType::Quadratic:
			equation = &CDBTweener::TWEQ_QUADRATIC;
			break;
		case TweenType::Cubic:
			equation = &CDBTweener::TWEQ_CUBIC;
			break;
		case TweenType::Quartic:
			equation = &CDBTweener::TWEQ_QUARTIC;
			break;
		case TweenType::Quintic:
			equation = &CDBTweener::TWEQ_QUINTIC;
			break;
		case TweenType::Exponential:
			equation = &CDBTweener::TWEQ_EXPONENTIAL;
			break;
		case TweenType::Circular:
			equation = &CDBTweener::TWEQ_CIRCULAR;
			break;
		case TweenType::Back:
			equation = &CDBTweener::TWEQ_BACK;
			break;
		case TweenType::Elastic:
			equation = &CDBTweener::TWEQ_ELASTIC;
			break;
		case TweenType::Bounce:
			equation = &CDBTweener::TWEQ_BOUNCE;
			break;
		}

		CDBTweener::EEasing easing = static_cast<CDBTweener::EEasing>(aMod);

		myTweener = new CDBTweener();
		myTweener->addTween(equation, easing, myDuration, &myValue, aEnd);
	}

	Tween::~Tween()
	{
		SAFE_DELETE(myTweener);
	}

	void Tween::Update(float aDeltaSeconds)
	{
		if (myTweener && !IsFinished())
		{
			myTweener->step(aDeltaSeconds);
			myProgress += aDeltaSeconds;
		}
	}

	float Tween::GetValue()
	{
		return myValue;
	}

	bool Tween::IsFinished()
	{
		return myProgress >= myDuration;
	}
}

#undef self
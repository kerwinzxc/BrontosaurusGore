#pragma once

class CDBTweener;

namespace CU
{
	enum class TweenType
	{
		Linear,
		Sinusoidal,
		Quadratic,
		Cubic,
		Quartic,
		Quintic,
		Exponential,
		Circular,
		Back,
		Elastic,
		Bounce
	};

	enum class TweenMod
	{
		EaseIn,
		EaseOut,
		EaseInOut
	};


	class Tween
	{
	public:
		Tween(TweenType aType, TweenMod aMod, float aStart, float aEnd, float aDuration);
		Tween(const Tween& aCopy) = delete;
		~Tween();

		Tween& operator=(const Tween& aCopy) = delete;

		void Update(float aDeltaSeconds);
		float GetValue();
		bool IsFinished();

	private:
		CDBTweener* myTweener;
		float myValue;
		float myProgress;
		float myDuration;
	};
}

#pragma once

enum ELUTType : char;


struct SColorFade
{
	float myFadeTime;
	ELUTType myFadeFrom;
	ELUTType myFadeTo;
};
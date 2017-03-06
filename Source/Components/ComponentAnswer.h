#pragma once


enum class eComponentQuestionType
{
	eHasCameraComponent,
	eGetCameraLookat,
	eGetCameraPosition,
	eLength,
};

struct SComponentQuestionData
{
	SComponentQuestionData() : myVector3f() {}

	union
	{
		CU::Vector3f myVector3f;
		bool myBool;
	};
};
#pragma once


enum class eComponentQuestionType
{
	eHasCameraComponent,
	eGetCameraLookat,
	eGetCameraPosition,
	eGetAmmoLeftString,
	eGetCameraObject,
	eLength,
};

struct SAmmoLeftData
{
	const char* weaponName;
	unsigned short ammoLeft;
	unsigned short maxAmmo;
};
struct SComponentQuestionData
{
	SComponentQuestionData() : myVector3f() {}

	union
	{
		CU::Vector3f myVector3f;
		bool myBool;
		const char* myString;
		SAmmoLeftData* myAmmoLeftData;
		CGameObject* myGameObject;
	};
};

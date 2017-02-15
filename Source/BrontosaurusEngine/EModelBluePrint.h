#pragma once

enum EModelBluePrint
{
	EModelBluePrint_Position = 1 << 0,
	EModelBluePrint_UV = 1 << 1,
	EModelBluePrint_Normal = 1 << 2,
	EModelBluePrint_BinormTan = 1 << 3,
	EModelBluePrint_Bones = 1 << 4,
	EModelBluePrint_Size = 1 << 5,
	EModelBluePrint_Alpha = 1 << 6,
	EModelBluePrint_Sprite2D = 1 << 7,
	EModelBluePrint_Text2D = 1 << 8,
	EModelBluePrint_Color = 1 << 9,
	EModelBluePrint_Fire = 1 << 10,
};

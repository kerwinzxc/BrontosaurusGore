#pragma once

struct SShape;
class CModelInstance;
class CModel;
class CSprite;

class CModelLoader
{
public:
	enum eTextureType
	{
		eAlbedo,
		eRoughness,
		eAmbientOcclusion,
		eEmissive,
		eNormal = 5,
		eMetalness = 10
	};

public:

	CModelLoader();
	~CModelLoader();

	static bool LoadModel(const std::string& aPath, CModel& aNewModel);
};


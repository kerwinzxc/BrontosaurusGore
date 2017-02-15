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

	static CModel* CreateShape(SShape aShape);
	static bool LoadModel(const char * aPath, CModel * aNewModel);

private:
	//static void CreateTriangle(CModel* aModel);
	//static void CreateQuad(CModel* aModel);
	static void CreateCube(CModel* aModel);
	static void CreateSphere(CModel* aModel, const float aSize);
};


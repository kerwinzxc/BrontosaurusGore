#include "stdafx.h"
#include "ModelLoader.h"
#include "Model.h"
#include "Sprite.h"
#include "ModelShapes.h"
#include "BufferStructs.h"
#include "FBXLoader.h"
#include "Effect.h"
#include "Engine.h"
#include "ShaderManager.h"
#include "Surface.h"

CModelLoader::CModelLoader()
{
}	

CModelLoader::~CModelLoader()
{
}

CModel* CModelLoader::CreateShape(SShape aShape)
{
	CModel* newModel = new CModel(); //TODO: MEMORY LEAK bc this is dereferenced and copied into myShapeList in model manager :(
	int blueprint = 1; // EModelBluePrint_Color | EModelBluePrint_Position;

	ID3D11VertexShader* vertexShader = SHADERMGR->LoadVertexShader(L"Shaders/vertex_shader.fx", blueprint);
	ID3D11PixelShader* pixelShader = SHADERMGR->LoadPixelShader(L"Shaders/pixel_shader.fx", blueprint);
	ID3D11GeometryShader* geometryShader = nullptr;// CEngine::GetInstance()->GetShaderManager()->LoadGeometryShader(L"Shaders/geometry_shader.fx", blueprint);
	ID3D11InputLayout* inputLayout = SHADERMGR->LoadInputLayout(L"Shaders/vertex_shader.fx", blueprint);
	D3D_PRIMITIVE_TOPOLOGY topology	= D3D10_PRIMITIVE_TOPOLOGY_LINESTRIP;
	//Put effects in a manager mebe?
	CEffect* effect = new CEffect(vertexShader, pixelShader, geometryShader, inputLayout, topology);

	switch (aShape.shape)
	{
	case eModelShape::eTriangle:
		assert(!"This shape is not implemented, and the function is out commented");
		//CreateTriangle(newModel);
		break;
	case eModelShape::eQuad:
		assert(!"This shape is not implemented, and the function is out commented");
		//CreateQuad(newModel);
		break;
	case eModelShape::eCube:
		CreateCube(newModel);
		break;
	case eModelShape::eSphere:
		CreateSphere(newModel, aShape.size);
		break;
	}

	newModel->Initialize(effect, nullptr);
	return newModel;
}

bool CModelLoader::LoadModel(const char* aPath, CModel* aNewModel) //TODO: FIX THIS SHIT.
{
	CFBXLoader loader;
	CLoaderScene scene;
	bool res = loader.LoadModelScene(aPath, scene);

	if (res == false)
	{
		//DL_ASSERT("Failed To load model with path: %s", aPath);
		return false;
	}

	if (scene.myMeshes.Size() == 0)
	{
		DL_ASSERT("Could not find meshes in file: %s", aPath);
		return false;
	}
	
	std::string modelPath = aPath;
	modelPath = modelPath.substr(0, ((modelPath.find_last_of('/') != std::wstring::npos) ? modelPath.find_last_of('/') : modelPath.find_last_of('\\')) + 1);
	std::wstring directory = std::wstring(modelPath.begin(), modelPath.end());

	int shaderType = scene.myMeshes[0]->myShaderType;
	std::string shaderPath = scene.myMeshes[0]->myShaderFile.c_str();
	std::wstring wShaderPath = std::wstring(shaderPath.begin(), shaderPath.end());

	ID3D11VertexShader* vertexShader = SHADERMGR->LoadVertexShader(wShaderPath != L"" ? directory + wShaderPath + L".fx" : L"Shaders/defered_vertex_shader.fx", shaderType);
	ID3D11PixelShader* pixelShader = SHADERMGR->LoadPixelShader(wShaderPath != L"" ? directory + wShaderPath + L".fx" : L"Shaders/defered_data_pixel_shader.fx", shaderType);



	ID3D11GeometryShader* geometryShader = nullptr;// CEngine::GetInstance()->GetShaderManager()->LoadGeometryShader(L"Shaders/geometry_shader.fx", shaderType);
	ID3D11InputLayout* inputLayout = SHADERMGR->LoadInputLayout(wShaderPath != L"" ? directory + wShaderPath + L".fx" : L"Shaders/Defered/vertex_shader.fx", shaderType);


	D3D_PRIMITIVE_TOPOLOGY topology			= D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//Put effects in a manager mebe?
	CEffect* effect = new CEffect(vertexShader, pixelShader, geometryShader, inputLayout, topology);
	CSurface* surface = new CSurface(modelPath, scene.myTextures);
	//CSurface* surface = new CSurface(texturePaths);

	if (scene.mySphereColData.radius > 0.0f)
	{
		aNewModel->mySphereColData = /*SSphereColData*/(scene.mySphereColData);
	}
	aNewModel->Initialize(effect, surface, scene.myMeshes);
	aNewModel->SetScene(scene.myScene);

	return true;
}

void CModelLoader::CreateCube(CModel* aModel)
{
	CU::GrowingArray<SVertexDataCube> modelVertices;// = aModel->GetVertices();
	CU::GrowingArray<unsigned int> modelIndices;// = aModel->GetIndices();

	modelVertices.Init(8);
	modelIndices.Init(36);

	float size = 500;

	modelVertices.Add(SVertexDataCube());
	modelVertices[0].position.x = -size;
	modelVertices[0].position.y = size;
	modelVertices[0].position.z = -size;
	modelVertices[0].position.w = 1.0f;

	//modelVertices[0].UV.u = 0.0f;
	//modelVertices[0].UV.v = 0.0f;

	modelVertices.Add(SVertexDataCube());
	modelVertices[1].position.x = -size;
	modelVertices[1].position.y = size;
	modelVertices[1].position.z = size;
	modelVertices[1].position.w = 1.0f;

	//modelVertices[1].UV.u = 1.0f;
	//modelVertices[1].UV.v = 0.0f;

	modelVertices.Add(SVertexDataCube());
	modelVertices[2].position.x = size;
	modelVertices[2].position.y = size;
	modelVertices[2].position.z = size;
	modelVertices[2].position.w = 1.0f;

	//modelVertices[2].UV.u = 0.0f;
	//modelVertices[2].UV.v = 0.0f;

	modelVertices.Add(SVertexDataCube());
	modelVertices[3].position.x = size;
	modelVertices[3].position.y = size;
	modelVertices[3].position.z = -size;
	modelVertices[3].position.w = 1.0f;

	//modelVertices[3].UV.u = 1.0f;
	//modelVertices[3].UV.v = 0.0f;

	// BOT
	modelVertices.Add(SVertexDataCube());
	modelVertices[4].position.x = -size;
	modelVertices[4].position.y = -size;
	modelVertices[4].position.z = -size;
	modelVertices[4].position.w = 1.0f;

	//modelVertices[4].UV.u = 0.0f;
	//modelVertices[4].UV.v = 1.0f;

	modelVertices.Add(SVertexDataCube());
	modelVertices[5].position.x = -size;
	modelVertices[5].position.y = -size;
	modelVertices[5].position.z = size;
	modelVertices[5].position.w = 1.0f;

	//modelVertices[5].UV.u = 1.0f;
	//modelVertices[5].UV.v = 1.0f;

	modelVertices.Add(SVertexDataCube());
	modelVertices[6].position.x = size;
	modelVertices[6].position.y = -size;
	modelVertices[6].position.z = size;
	modelVertices[6].position.w = 1.0f;

	//modelVertices[6].UV.u = 0.0f;
	//modelVertices[6].UV.v = 1.0f;

	modelVertices.Add(SVertexDataCube());
	modelVertices[7].position.x = size;
	modelVertices[7].position.y = -size;
	modelVertices[7].position.z = -size;
	modelVertices[7].position.w = 1.0f;

	//modelVertices[7].UV.u = 1.0f;
	//modelVertices[7].UV.v = 1.0f;


	modelIndices.Add(0);
	modelIndices.Add(1);
	modelIndices.Add(2);
	modelIndices.Add(0);
	modelIndices.Add(2);
	modelIndices.Add(3);

	modelIndices.Add(4);
	modelIndices.Add(6);
	modelIndices.Add(5);
	modelIndices.Add(4);
	modelIndices.Add(7);
	modelIndices.Add(6);
	
	modelIndices.Add(4);
	modelIndices.Add(5);
	modelIndices.Add(1);
	modelIndices.Add(4);
	modelIndices.Add(1);
	modelIndices.Add(0);
	
	modelIndices.Add(6);
	modelIndices.Add(7);
	modelIndices.Add(3);
	modelIndices.Add(6);
	modelIndices.Add(3);
	modelIndices.Add(2);

	modelIndices.Add(4);
	modelIndices.Add(0);
	modelIndices.Add(3);
	modelIndices.Add(4);
	modelIndices.Add(3);
	modelIndices.Add(7);
	
	modelIndices.Add(6);
	modelIndices.Add(2);
	modelIndices.Add(1);
	modelIndices.Add(6);
	modelIndices.Add(1);
	modelIndices.Add(5);

	aModel->InitBuffers(modelVertices, modelIndices);
}

void CModelLoader::CreateSphere(CModel* aModel, const float aSize)
{
	CU::GrowingArray<SVertexDataCube> modelVertices;
	modelVertices.Init(3000);

	int radius = static_cast<int>(aSize);
	//const float PI = 3.1415926535f;
	int index = 0;


	for (float theta = 0.; theta < PI; theta += PI / 10.)
	{
		for (float phi = 0.; phi < 2 * PI; phi += PI / 10.)
		{
			CU::Vector4f point;
			point.x = radius * cos(phi) * sin(theta);
			point.y = radius * sin(phi) * sin(theta);
			point.z = radius            * cos(theta);
			point.w = 1.f;
			modelVertices.Add(SVertexDataCube());
			modelVertices[index].position = point;
			index++;
		}
	}
	aModel->InitBuffers(modelVertices);
}


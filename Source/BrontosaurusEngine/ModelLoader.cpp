#include "stdafx.h"
#include "ModelLoader.h"
#include "Model.h"
#include "Sprite.h"
#include "BufferStructs.h"
#include "FBXLoader.h"
#include "Effect.h"
#include "Engine.h"
#include "ShaderManager.h"
#include "Surface.h"

constexpr const char* MODEL_TEXTURE_DIRECTORY = "Models/Textures/";

CModelLoader::CModelLoader()
{

}

CModelLoader::~CModelLoader()
{

}

bool CModelLoader::LoadModel(const std::string& aPath, CModel& aNewModel) //TODO: FIX THIS SHIT.
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
		DL_ASSERT("Could not find meshes in file: %s", aPath.c_str());
		return false;
	}
	
	std::string modelPath = aPath;
	size_t directoryEnd = modelPath.find_last_of('/');
	if (directoryEnd == std::string::npos)
	{
		modelPath.find_last_of('\\');
	}
	
	modelPath = modelPath.substr(0, directoryEnd + 1);
	std::wstring directory(modelPath.begin(), modelPath.end());

	int shaderType = scene.myMeshes[0]->myShaderType;

	const std::string& shaderPath = scene.myMeshes[0]->myShaderFile;
	std::wstring wShaderPath(shaderPath.begin(), shaderPath.end());

	ID3D11VertexShader* vertexShader = SHADERMGR->LoadVertexShader(wShaderPath != L"" ? directory + wShaderPath + L".fx" : L"Shaders/vertex_shader.fx", shaderType);

	ID3D11VertexShader* vertexInstancedShader = nullptr;
	ID3D11PixelShader* pixelInstancedShader = nullptr;
	ID3D11InputLayout* instancedInputLayout = nullptr;
	if (scene.myNumBones == 0)
	{
		vertexInstancedShader = SHADERMGR->LoadVertexShader(wShaderPath != L"" ? directory + wShaderPath + L".fx" : L"Shaders/vertex_shader.fx", shaderType | EModelBluePrint_Instance);
		instancedInputLayout = SHADERMGR->LoadInputLayout(wShaderPath != L"" ? directory + wShaderPath + L".fx" : L"Shaders/vertex_shader.fx", shaderType | EModelBluePrint_Instance);
		pixelInstancedShader = SHADERMGR->LoadPixelShader(wShaderPath != L"" ? directory + wShaderPath + L".fx" : L"Shaders/Deferred/deferred_pixel.fx", shaderType | EModelBluePrint_Instance);
	}

	ID3D11PixelShader* forwardPixelShader = SHADERMGR->LoadPixelShader(wShaderPath != L"" ? directory + wShaderPath + L".fx" : L"Shaders/pixel_shader.fx", shaderType);
	ID3D11PixelShader* deferredPixelShader = SHADERMGR->LoadPixelShader(wShaderPath != L"" ? directory + wShaderPath + L".fx" : L"Shaders/Deferred/deferred_pixel.fx", shaderType);

	ID3D11GeometryShader* geometryShader = nullptr;// CEngine::GetInstance()->GetShaderManager()->LoadGeometryShader(L"Shaders/geometry_shader.fx", shaderType);
	ID3D11InputLayout* inputLayout = SHADERMGR->LoadInputLayout(wShaderPath != L"" ? directory + wShaderPath + L".fx" : L"Shaders/vertex_shader.fx", shaderType);

	D3D_PRIMITIVE_TOPOLOGY topology = D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	//Put effects in a manager mebe?
	CEffect* forwardEffect = new CEffect(vertexShader, forwardPixelShader, geometryShader, inputLayout, topology, vertexInstancedShader, instancedInputLayout, pixelInstancedShader);
	CEffect* deferredEffect = new CEffect(vertexShader, deferredPixelShader, geometryShader, inputLayout, topology, vertexInstancedShader, instancedInputLayout, pixelInstancedShader);

	CSurface* surface = new CSurface(MODEL_TEXTURE_DIRECTORY, scene.myTextures);

	aNewModel.Initialize(forwardEffect, surface, scene.myMeshes);
	aNewModel.myDeferredEffect = deferredEffect;
	aNewModel.SetScene(scene.myScene);

	return true;
}

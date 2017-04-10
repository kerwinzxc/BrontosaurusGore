#include "stdafx.h"
#include <string>
#include "ParticleEmitter.h"

#include "JsonValue.h"
#include "EmitterData.h"

#include "ShaderManager.h"
#include "Effect.h"
#include "VertexStructs.h"

#include "DXFramework.h"
#include "ConstBufferTemplate.h"
#include "TextureManager.h"
#include "ParticleEmitterInstance.h"
#include "../Particles/IParticleSpawner.h"
#include "../Particles/ParticleColorSpawner.h"
#include "../Particles/ParticleVelocitySpawner.h"
#include "../Particles/ParticleLifetimeSpawner.h"
#include "../Particles/ParticleSizeSpawner.h"
#include "../Particles/ParticleForceUpdater.h"



CParticleEmitter::CParticleEmitter()
{
	myRefCount = 0;
	myEmitterData.emitter.maxNrOfParticles = 0;
	myEmitterData.render.myTexture = nullptr;

	for (int i = 0; i < myRenderEffects.Size(); ++i)
	{
		myRenderEffects[i] = nullptr;
	}

	myFramework = nullptr;
	myVertexBuffer = nullptr;
	myModelBuffer = nullptr;
	myEmitterData.particles.updaters.Init(5);
	myEmitterData.particles.spawners.Init(5);

}

CParticleEmitter::CParticleEmitter(const CU::CJsonValue& aJsonValue) : CParticleEmitter()
{
	myEmitterData.name = aJsonValue["name"].GetString();
	myEmitterData.id = aJsonValue["id"].GetInt();

	myEmitterData.emitter.maxNrOfParticles = aJsonValue["maxNumOfParticles"].GetUInt();
	myEmitterData.emitter.emissionRate = aJsonValue["emissionRate"].GetUInt();

	myEmitterData.emitter.loop = aJsonValue["loop"].GetBool();
	myEmitterData.emitter.lifetime = aJsonValue["lifetime"].GetFloat();
	
	ParseEmissionArea(aJsonValue["emitter"]);
	ParseParticle(aJsonValue["particle"]);


	Init();
}

CParticleEmitter::~CParticleEmitter()
{
	Destroy();
}

void CParticleEmitter::Render(const CU::Matrix44f & aToWorldSpace, const CU::GrowingArray<SParticle, unsigned int, false>& aParticleList, RenderMode aRenderMode)
{
	const RenderMode renderMode = myEmitterData.render.renderMode;
	if (!myRenderEffects[static_cast<int>(myEmitterData.render.renderMode)]) return;

	myRenderEffects[static_cast<int>(aRenderMode)]->Activate();
	UpdateCBuffers(aToWorldSpace);
	ResizeVertexBuffer(aParticleList);

	if(myEmitterData.render.myTexture != nullptr)
	{
		DEVICE_CONTEXT->PSSetShaderResources(1, 1, myEmitterData.render.myTexture->GetShaderResourceViewPointer());
	}

	UINT stride = sizeof(SParticle);
	UINT offset = 0;
	ID3D11Buffer* vertexBuffer = GetVertexBuffer();
	myFramework->GetDeviceContext()->IASetVertexBuffers(0, 1, &vertexBuffer, &stride, &offset);
	myFramework->GetDeviceContext()->Draw(aParticleList.Size(), 0);
}

void CParticleEmitter::Destroy()
{
	if (myEmitterData.render.myTexture)
	{
		CTextureManager& textureManager = TEXTUREMGR;
		textureManager.DestroyTexture(myEmitterData.render.myTexture);
	}

	SAFE_RELEASE(myModelBuffer);
	SAFE_RELEASE(myVertexBuffer);
	for(int i = 0; i < myRenderEffects.Size(); ++i)
	{
		SAFE_DELETE(myRenderEffects[i]);
	}

}

bool CParticleEmitter::HasEffects() const
{
	for(int i = 0; i < myRenderEffects.Size(); ++i)
	{
		if(myRenderEffects[i] != nullptr)
		{
			return true;
		}
	}
	return false;
}

void CParticleEmitter::AddRef()
{
	++myRefCount;
}

void CParticleEmitter::RemoveRef()
{
	--myRefCount;
}

void CParticleEmitter::UpdateInstance(const CU::Time& aTime, CParticleEmitterInstance& aInstance)
{

	myCurrentInstaceTransform = aInstance.myToWorldSpace;
	if(aInstance.IsActive() == false && myEmitterData.emitter.loop == false)
	{
		return;
	}

	const float dt = aTime.GetSeconds();
	aInstance.myLifetime -= dt;

	if(aInstance.myLifetime < 0.f && myEmitterData.emitter.loop == false)
	{
		aInstance.Deactivate();
		return;
	}

	aInstance.myEmitTimer -= dt;

	if(aInstance.myEmitTimer < 0)
	{
		aInstance.ResetSpawnTimer();
		if(aInstance.myParticles.Size() < myEmitterData.emitter.maxNrOfParticles)
		{
			SParticle particle;
			particle.position = aInstance.myToWorldSpace.GetPosition();
			SParticleLogic particleLogic;

			SpawnParticle(particle, particleLogic);

			aInstance.myParticles.Add(particle);
			aInstance.myParticleLogic.Add(particleLogic);
		}
	}

	for(int i = aInstance.myParticles.Size() - 1; i >= 0; --i)
	{
		SParticle& particle = aInstance.myParticles[i];
		SParticleLogic& logic = aInstance.myParticleLogic[i];

		logic.lifetimeLeft -= dt;

		if(logic.lifetimeLeft < 0.f)
		{
			aInstance.myParticles.RemoveCyclicAtIndex(i);
			aInstance.myParticleLogic.RemoveCyclicAtIndex(i);
		}

		UpdateParticles(particle, logic, dt);
	}

}

float CParticleEmitter::GetLifetime() const
{
	return myEmitterData.emitter.lifetime;
}

float CParticleEmitter::GetEmitTime()
{
	return 1.f / myEmitterData.emitter.emissionRate;
}

unsigned CParticleEmitter::GetMaxParticles()
{
	return myEmitterData.emitter.maxNrOfParticles;
}

void CParticleEmitter::Init()
{
	unsigned int ShaderType = 0;
	ShaderType |= EModelBluePrint_Position;
	ShaderType |= EModelBluePrint_Size;
	ShaderType |= EModelBluePrint_Color;

	ID3D11VertexShader* vertexShader = SHADERMGR->LoadVertexShader(L"Shaders/metaballs/metaballsVertex.fx", ShaderType);
	ID3D11PixelShader* pixelShader = SHADERMGR->LoadPixelShader(L"Shaders/metaballs/metaballsSpherePixel.fx", ShaderType);
	ID3D11GeometryShader* geometryShader = SHADERMGR->LoadGeometryShader(L"Shaders/metaballs/metaballsGeometry.fx", ShaderType);
	ID3D11InputLayout* inputLayout = SHADERMGR->LoadInputLayout(L"Shaders/metaballs/metaballsVertex.fx", ShaderType);

	myRenderEffects[static_cast<int>(RenderMode::eMetaBall)] = new CEffect(vertexShader, pixelShader, geometryShader, inputLayout, D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	vertexShader = SHADERMGR->LoadVertexShader(L"Shaders/nurbs/nurbsVertex.fx", ShaderType);
	pixelShader = SHADERMGR->LoadPixelShader(L"Shaders/nurbs/nurbsSpherePixel.fx", ShaderType);
	geometryShader = SHADERMGR->LoadGeometryShader(L"Shaders/nurbs/nurbsGeometry.fx", ShaderType);
	inputLayout = SHADERMGR->LoadInputLayout(L"Shaders/nurbs/nurbsVertex.fx", ShaderType);

	myRenderEffects[static_cast<int>(RenderMode::eNURBSSphere)] =
		new CEffect(vertexShader, pixelShader, geometryShader, inputLayout, D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST);
	
	vertexShader = SHADERMGR->LoadVertexShader(L"Shaders/particles/billboardVertex.fx", ShaderType);
	pixelShader = SHADERMGR->LoadPixelShader(L"Shaders/particles/billboardPixel.fx", ShaderType);
	geometryShader = SHADERMGR->LoadGeometryShader(L"Shaders/particles/billboardGeometry.fx", ShaderType);
	inputLayout = SHADERMGR->LoadInputLayout(L"Shaders/particles/billboardVertex.fx", ShaderType);

	myRenderEffects[static_cast<int>(RenderMode::eBillboard)] =
		new CEffect(vertexShader, pixelShader, geometryShader, inputLayout, D3D_PRIMITIVE_TOPOLOGY::D3D_PRIMITIVE_TOPOLOGY_POINTLIST);

	myFramework = FRAMEWORK;
	InitBuffers();
}

void CParticleEmitter::InitialVelocity(SParticleLogic& aParticleLogic)
{
	const CU::Vector3f forward = -myCurrentInstaceTransform.myForwardVector;

	const float spread = myEmitterData.emitter.spread.value.angle;
	const float halfSpread = spread / 2.f;
	const float xRot = halfSpread - (static_cast<float>(rand()) / RAND_MAX) * spread;
	const float yRot = halfSpread - (static_cast<float>(rand()) / RAND_MAX) * spread;
	const float zRot = halfSpread - (static_cast<float>(rand()) / RAND_MAX) * spread;

	aParticleLogic.movementDir = forward * CU::Matrix33f::CreateRotateAroundX(xRot) * CU::Matrix33f::CreateRotateAroundY(yRot) * CU::Matrix33f::CreateRotateAroundZ(zRot);
}

void CParticleEmitter::SpawnParticle(SParticle& aParticle, SParticleLogic& aParticleLogic)
{
	InitialVelocity(aParticleLogic);
	for(int i = 0; i < myEmitterData.particles.spawners.Size(); ++i)
	{
		myEmitterData.particles.spawners[i]->Spawn(aParticle, aParticleLogic);
	}
}

void CParticleEmitter::UpdateParticles(SParticle& aParticle, SParticleLogic& aLogic, const float aDt)
{
	for(int i = 0;i < myEmitterData.particles.updaters.Size(); ++i)
	{
		myEmitterData.particles.updaters[i]->Update(aDt, aParticle, aLogic);
	}

	aParticle.position += aLogic.movementDir * aLogic.speed * aDt;
}

void CParticleEmitter::ParseSpawnParameters(const CU::CJsonValue& aJsonValue)
{
	for (int i = 0; i < aJsonValue.Size(); ++i)
	{
		Particles::IParticleSpawner* spawner = nullptr;
		const CU::CJsonValue value = aJsonValue[i];

		const std::string type = value["valueType"].GetString();

		if (type == "velocity")
		{
			spawner = new Particles::CParticleVelocitySpawner(value);
		}
		else if (type == "size")
		{
			spawner = new Particles::CParticleSizeSpawner(value);
		}
		else if (type == "color")
		{
			spawner = new Particles::CParticleColorSpawner(value);
		}
		else if(type == "lifetime")
		{
			spawner = new Particles::CParticleLifetimeSpawner(value);
		}
		else
		{
			DL_ASSERT("Attempting to add a unknows spawn parameter \"%s\"", type.c_str());
		}
		
		myEmitterData.particles.spawners.Add(spawner);
	}
}

void CParticleEmitter::ParseUpdateParameters(const CU::CJsonValue& aJsonValue)
{
	for (int i = 0; i < aJsonValue.Size(); ++i)
	{
		Particles::IParticleUpdater* updater = nullptr;
		const CU::CJsonValue value = aJsonValue[i];

		const std::string type = value["valueType"].GetString();

		if (type == "force")
		{
			updater = new Particles::CParticleForceUpdater(value);
		}
		else if (type == "friction")
		{
			continue;
			//spawner = new Particles::CParticleSizeSpawner(value);
		}
		else
		{
			DL_ASSERT("Attempting to add a unknows update parameter \"%s\"", type.c_str());
		}

		myEmitterData.particles.updaters.Add(updater);
	}
}

void CParticleEmitter::ParseParticle(const CU::CJsonValue& aJsonValue)
{
	ParseRender(aJsonValue["render"]);
	//myEmitterData.particles.spawners.Add(new Particles::CParticleLifetimeSpawner(aJsonValue["lifetime"]));
	ParseSpawnParameters(aJsonValue["spawn"]);
	ParseUpdateParameters(aJsonValue["update"]);
}

CParticleEmitter::EmitterType CParticleEmitter::ParseEmitterType(const std::string& aTypeString)
{
	if(aTypeString == "point")
	{
		return EmitterType::ePoint;
	}

	DL_ASSERT("Unknown emitter type \"%s\"", aTypeString.c_str());
	return EmitterType::eNone;
}

CParticleEmitter::SpreadType CParticleEmitter::GetSpreadType(const std::string& aTypeString)
{
	if(aTypeString == "sphere")
	{
		return SpreadType::eSphere;
	}

	DL_ASSERT("Unknown spreadtype \"%s\"", aTypeString.c_str());
	return SpreadType::eNone;
}

void CParticleEmitter::ParseEmissionArea(const CU::CJsonValue& aJsonValue)
{
	const EmitterType type = ParseEmitterType(aJsonValue["type"].GetString());
	myEmitterData.emitter.type = type;
	switch (type)
	{
	case EmitterType::ePoint: 
		break;
	case EmitterType::eSphere:
	case EmitterType::eNone:
	default: 
		DL_ASSERT("No handling for emitter type \"%s\"", aJsonValue["type"].GetString().c_str());
		break;
	}

	const CU::CJsonValue spread = aJsonValue["spread"];
	const SpreadType spreadType = GetSpreadType(spread["type"].GetString());
	//myEmitterData.emitter.spread.type = spreadType;

	switch (spreadType)
	{
	case SpreadType::eSphere: 
		myEmitterData.emitter.spread.value.angle = spread["angle"].GetFloat();
		break;
	case SpreadType::eNone: 
	default: 
		DL_ASSERT("No handling for spread type \"%s\"",spread["type"].GetString().c_str());
		break;
	}
}

CParticleEmitter::RenderMode CParticleEmitter::GetRenderMode(const std::string& aRenderModeString)
{
	if(aRenderModeString == "sprite")
	{
		return RenderMode::eBillboard;
	}
	if(aRenderModeString == "metaball")
	{
		return RenderMode::eMetaBall;
	}
	if(aRenderModeString == "nurbssphere")
	{
		return RenderMode::eNURBSSphere;
	}

	DL_ASSERT("Unkown render mode type \"%s\" in emitter \"%s\"", aRenderModeString.c_str(), myEmitterData.name.c_str());
	return RenderMode::eBillboard;
}

CParticleEmitter::RenderMode CParticleEmitter::GetRenderMode() const
{
	return myEmitterData.render.renderMode;
}

void CParticleEmitter::ParseRender(const CU::CJsonValue& aJsonValue)
{
	myEmitterData.render.renderMode = GetRenderMode(aJsonValue["type"].GetString());

	myEmitterData.render.color = aJsonValue["color"].GetVector4f("rgba");

	if(myEmitterData.render.renderMode == RenderMode::eBillboard)
	{
		myEmitterData.render.myTexture = &TEXTUREMGR.LoadTexture(aJsonValue["path"].GetString().c_str());;
	}
}

ID3D11Buffer* CParticleEmitter::GetVertexBuffer()
{
	std::lock_guard<std::mutex> lock(myUpdateVBufferMutex);
	return myVertexBuffer;
}

void CParticleEmitter::ResizeVertexBuffer(const CU::GrowingArray<SParticle, unsigned int, false>& aParticleList)
{
	if (aParticleList.Empty()) return;

	unsigned int bufferSize = sizeof(SParticle);

	if (aParticleList.Size() > myEmitterData.emitter.maxNrOfParticles)
	{
		bufferSize *= myEmitterData.emitter.maxNrOfParticles;
	}
	else
	{
		bufferSize *= aParticleList.Size();
	}

	D3D11_MAPPED_SUBRESOURCE resource;
	ZeroMemory(&resource, sizeof(D3D11_MAPPED_SUBRESOURCE));
	
	DEVICE_CONTEXT->Map(myVertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &resource);
	std::lock_guard<std::mutex> lock(myUpdateVBufferMutex);
	memcpy(resource.pData, &aParticleList[0], bufferSize);

	DEVICE_CONTEXT->Unmap(myVertexBuffer, 0);
}

void CParticleEmitter::UpdateCBuffers(const CU::Matrix44f & aToWorldSpace)
{
	//WorldSpace
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;
	SToWorldSpace updatedWorld;
	updatedWorld.myWorldSpace = aToWorldSpace;

	DEVICE_CONTEXT->Map(myModelBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedSubresource);
	memcpy(mappedSubresource.pData, &updatedWorld, sizeof(SToWorldSpace));
	DEVICE_CONTEXT->Unmap(myModelBuffer, 0);
	DEVICE_CONTEXT->VSSetConstantBuffers(1, 1, &myModelBuffer);
}

bool CParticleEmitter::InitBuffers()
{
	//VertexBuffer
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(SParticle) * myEmitterData.emitter.maxNrOfParticles;
	vertexBufferDesc.BindFlags = D3D11_USAGE_DYNAMIC | D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vertexBufferDesc.MiscFlags = 0;

	HRESULT result;
	result = DEVICE->CreateBuffer(&vertexBufferDesc, nullptr, &myVertexBuffer);
	CHECK_RESULT(result, "Failed to create vertex buffer.");

	SAFE_RELEASE(myModelBuffer);
	//Cbuffers
	SToWorldSpace worldBufferData;
	myModelBuffer = BSR::CreateCBuffer<SToWorldSpace>(&worldBufferData);

	return true;
}

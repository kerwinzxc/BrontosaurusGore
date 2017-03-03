#include "stdafx.h"
#include "ApexRenderResourceManager.h"

Physics::Apex::CApexRenderResourceManager::CApexRenderResourceManager(ID3D11Device* aDevicePtr) : myDevice(aDevicePtr)
{
}

Physics::Apex::CApexRenderResourceManager::~CApexRenderResourceManager()
{
}

nvidia::UserRenderVertexBuffer* Physics::Apex::CApexRenderResourceManager::createVertexBuffer(const nvidia::UserRenderVertexBufferDesc& desc)
{
	return nullptr;
}

void Physics::Apex::CApexRenderResourceManager::releaseVertexBuffer(nvidia::UserRenderVertexBuffer& buffer)
{
}

nvidia::UserRenderIndexBuffer* Physics::Apex::CApexRenderResourceManager::createIndexBuffer(const nvidia::UserRenderIndexBufferDesc& desc)
{
	return nullptr;
}

void Physics::Apex::CApexRenderResourceManager::releaseIndexBuffer(nvidia::UserRenderIndexBuffer& buffer)
{
}

nvidia::UserRenderBoneBuffer* Physics::Apex::CApexRenderResourceManager::createBoneBuffer(const nvidia::UserRenderBoneBufferDesc& desc)
{
	return nullptr;
}

void Physics::Apex::CApexRenderResourceManager::releaseBoneBuffer(nvidia::UserRenderBoneBuffer& buffer)
{
}

nvidia::UserRenderInstanceBuffer* Physics::Apex::CApexRenderResourceManager::createInstanceBuffer(const nvidia::UserRenderInstanceBufferDesc& desc)
{
	return nullptr;
}

void Physics::Apex::CApexRenderResourceManager::releaseInstanceBuffer(nvidia::UserRenderInstanceBuffer& buffer)
{
}

nvidia::UserRenderSpriteBuffer* Physics::Apex::CApexRenderResourceManager::createSpriteBuffer(const nvidia::UserRenderSpriteBufferDesc& desc)
{
	return nullptr;
}

void Physics::Apex::CApexRenderResourceManager::releaseSpriteBuffer(nvidia::UserRenderSpriteBuffer& buffer)
{
}

nvidia::UserRenderSurfaceBuffer* Physics::Apex::CApexRenderResourceManager::createSurfaceBuffer(const nvidia::UserRenderSurfaceBufferDesc& desc)
{
	return nullptr;
}

void Physics::Apex::CApexRenderResourceManager::releaseSurfaceBuffer(nvidia::UserRenderSurfaceBuffer& buffer)
{
}

nvidia::UserRenderResource* Physics::Apex::CApexRenderResourceManager::createResource(const nvidia::UserRenderResourceDesc& desc)
{
	return nullptr;
}

void Physics::Apex::CApexRenderResourceManager::releaseResource(nvidia::UserRenderResource& resource)
{
}

uint32_t Physics::Apex::CApexRenderResourceManager::getMaxBonesForMaterial(void* material)
{
	return 0;
}

bool Physics::Apex::CApexRenderResourceManager::getSpriteLayoutData(uint32_t spriteCount, uint32_t spriteSemanticsBitmap, nvidia::UserRenderSpriteBufferDesc* textureDescArray)
{
	return false;
}

bool Physics::Apex::CApexRenderResourceManager::getInstanceLayoutData(uint32_t spriteCount, uint32_t spriteSemanticsBitmap, nvidia::UserRenderInstanceBufferDesc* instanceDescArray)
{
	return false;
}

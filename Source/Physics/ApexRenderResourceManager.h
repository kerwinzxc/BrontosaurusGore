#pragma once
#include "UserRenderResourceManager.h"
#include <d3d11.h>

class ID3D11Device;

namespace Physics
{
	namespace Apex
	{
		class CApexRenderResourceManager : public nvidia::apex::UserRenderResourceManager
		{
		public:
			CApexRenderResourceManager(ID3D11Device* aDevicePtr);
			~CApexRenderResourceManager();
			nvidia::UserRenderVertexBuffer* createVertexBuffer(const nvidia::UserRenderVertexBufferDesc& desc) override;
			void releaseVertexBuffer(nvidia::UserRenderVertexBuffer& buffer) override;
			nvidia::UserRenderIndexBuffer* createIndexBuffer(const nvidia::UserRenderIndexBufferDesc& desc) override;
			void releaseIndexBuffer(nvidia::UserRenderIndexBuffer& buffer) override;
			nvidia::UserRenderBoneBuffer* createBoneBuffer(const nvidia::UserRenderBoneBufferDesc& desc) override;
			void releaseBoneBuffer(nvidia::UserRenderBoneBuffer& buffer) override;
			nvidia::UserRenderInstanceBuffer* createInstanceBuffer(const nvidia::UserRenderInstanceBufferDesc& desc) override;
			void releaseInstanceBuffer(nvidia::UserRenderInstanceBuffer& buffer) override;
			nvidia::UserRenderSpriteBuffer* createSpriteBuffer(const nvidia::UserRenderSpriteBufferDesc& desc) override;
			void releaseSpriteBuffer(nvidia::UserRenderSpriteBuffer& buffer) override;
			nvidia::UserRenderSurfaceBuffer* createSurfaceBuffer(const nvidia::UserRenderSurfaceBufferDesc& desc) override;
			void releaseSurfaceBuffer(nvidia::UserRenderSurfaceBuffer& buffer) override;
			nvidia::UserRenderResource* createResource(const nvidia::UserRenderResourceDesc& desc) override;
			void releaseResource(nvidia::UserRenderResource& resource) override;
			uint32_t getMaxBonesForMaterial(void* material) override;
			bool getSpriteLayoutData(uint32_t spriteCount, uint32_t spriteSemanticsBitmap, nvidia::UserRenderSpriteBufferDesc* textureDescArray) override;
			bool getInstanceLayoutData(uint32_t spriteCount, uint32_t spriteSemanticsBitmap, nvidia::UserRenderInstanceBufferDesc* instanceDescArray) override;
		protected:
			ID3D11Device* myDevice;
		};
	}
}


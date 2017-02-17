#include "stdafx.h"
#include "PointLight.h"

#define POINTLIGHT_SHADOW_RESOLUTION 1024.f


CPointLight::CPointLight(const CU::Vector4f& aColor, const float aRange, const float aIntensity)
{
	myColor = aColor;
	myIntensity = aIntensity;
	myRange = aRange;

	//for (int i = 0; i < myRenderCameras.Size(); ++i)
	//{
	//	myRenderCameras[i] = CRenderCamera();
	//	myRenderCameras[i].InitPerspective(90.f, POINTLIGHT_SHADOW_RESOLUTION, POINTLIGHT_SHADOW_RESOLUTION, aRange, 10.0f);
	//}


	//myShadowCubeMap = nullptr;

	//myRenderCameras[ePointLightFace::eBack].GetCamera().Jaw(3.1415f);
	//myRenderCameras[ePointLightFace::eLeft].GetCamera().Jaw(3.1415f / 2.f);
	//myRenderCameras[ePointLightFace::eRight].GetCamera().Jaw(-3.1415f / 2.f);
	//myRenderCameras[ePointLightFace::eUP].GetCamera().Pitch(3.1415f / 2.0f);
	//myRenderCameras[ePointLightFace::eDown].GetCamera().Pitch(3.1415f / 2.0f);


}


CPointLight::~CPointLight()
{
}


void CPointLight::RenderShadowMap(const CU::Vector4f& /*aPosition*/)
{
	//SAFE_RELEASE(myShadowCubeMap);



	//for (int i = 0; i < myRenderCameras.Size(); ++i)
	//{
	//	myRenderCameras[i].Render();
	//}

	////ID3D11Texture2D* cubeTexture;

	//D3D11_TEXTURE2D_DESC texDesc;
	//texDesc.Width = POINTLIGHT_SHADOW_RESOLUTION;
	//texDesc.Height = POINTLIGHT_SHADOW_RESOLUTION;
	//texDesc.MipLevels = 1;
	//texDesc.ArraySize = 6;
	//texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	//texDesc.CPUAccessFlags = 0;
	//texDesc.SampleDesc.Count = 1;
	//texDesc.SampleDesc.Quality = 0;
	//texDesc.Usage = D3D11_USAGE_DEFAULT;
	//texDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	//texDesc.CPUAccessFlags = 0;
	//texDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;

	//D3D11_SHADER_RESOURCE_VIEW_DESC SMViewDesc;
	//SMViewDesc.Format = texDesc.Format;
	//SMViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
	//SMViewDesc.TextureCube.MipLevels = texDesc.MipLevels;
	//SMViewDesc.TextureCube.MostDetailedMip = 0;

	// //Array to fill which we will use to point D3D at our loaded CPU images.
	// D3D11_SUBRESOURCE_DATA pData[6];
	// for (int cubeMapFaceIndex = 0; cubeMapFaceIndex < 6; cubeMapFaceIndex++)
	// {
	//	 D3D11_MAP eMapType = D3D11_MAP_READ;
	//	 D3D11_MAPPED_SUBRESOURCE mappedResource;
	//	 ID3D11Texture2D* camtexture = myRenderCameras[cubeMapFaceIndex].GetRenderPackage().GetTexture();


	//	 DEVICE_CONTEXT->Map(camtexture, 0, eMapType, NULL, &mappedResource);

	//	 BYTE* pYourBytes = (BYTE*)mappedResource.pData;
	//	 unsigned int uiPitch = mappedResource.RowPitch;


	//	 //Pointer to the pixel data
	//	 pData[cubeMapFaceIndex].pSysMem = pYourBytes;
	//	 //Line width in bytes
	//	 pData[cubeMapFaceIndex].SysMemPitch = uiPitch; // 4 for DXGI_FORMAT_R8G8B8A8_UNORM
	//	 // This is only used for 3d textures.
	//	 pData[cubeMapFaceIndex].SysMemSlicePitch = 0;
	//	 
	//	 
	//	 DEVICE_CONTEXT->Unmap(camtexture, 0);
	// }


}


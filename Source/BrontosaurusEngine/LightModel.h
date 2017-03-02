#pragma once

class CLoaderMesh;

class CLightModel
{
public:
	CLightModel();
	~CLightModel();

	void Init(CEffect* aEffect, const CLoaderMesh* aLoadedMesh);
	void Render(const CU::Matrix44f& aTransformation);

private:
	CEffect* myEffect;

	ID3D11Buffer* myVertexBuffer;
	ID3D11Buffer* myIndexBuffer;
	ID3D11Buffer* myWorldSpaceBuffer;

	unsigned int myVertexCount;
	unsigned int myVertexSize;
	unsigned int myIndexCount;
};



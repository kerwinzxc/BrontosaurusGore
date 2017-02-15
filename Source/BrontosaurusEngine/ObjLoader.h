#pragma once
#include <GrowingArray.h>
#include <Vector2.h>
#include <Vector3.h>



namespace ObjLoader
{


	struct SObjLoaderVertex
	{

		SObjLoaderVertex()
		{
			VertexIndex = UVIndex = NormalIndex = 0;
		}

		SObjLoaderVertex(unsigned int vi, unsigned int UVi, unsigned int ni)
		{
			VertexIndex = vi;
			UVIndex = UVi;
			NormalIndex = ni;
		};

		unsigned int VertexIndex, UVIndex, NormalIndex;
	};


	struct SObjLoaderFace
	{
		SObjLoaderFace() {}
		SObjLoaderFace(const SObjLoaderVertex& aFirst, const SObjLoaderVertex& aSecond, const SObjLoaderVertex& aThird)
		{
			myVerteces[0] = aFirst;
			myVerteces[1] = aSecond;
			myVerteces[2] = aThird;
		}

		SObjLoaderVertex myVerteces[3];
	};


	struct SObjLoaderModel
	{
		const char* path;
		CU::GrowingArray<CU::Vector3f> VerticesList;
		CU::GrowingArray<CU::Vector2f> UVList;
		CU::GrowingArray<CU::Vector3f> NormalsList;
		CU::GrowingArray<SObjLoaderFace> FaceList;


		SObjLoaderModel()
		{
			path = nullptr;
			FaceList.Init(32);
			VerticesList.Init(32);
			NormalsList.Init(32);
			UVList.Init(32);
		}
	};



	bool LoadFromFile(const char * const aModelPath, SObjLoaderModel & aModelOut);

}




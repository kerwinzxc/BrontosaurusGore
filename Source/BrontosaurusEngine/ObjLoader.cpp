#include "stdafx.h"
#include "ObjLoader.h"
#include <fstream>

#include <DL_Debug.h>

#pragma warning (push)
#pragma warning (disable : 4996)

#define OBJ_SCALE 100

bool ObjLoader::LoadFromFile(const char * const aModelPath, SObjLoaderModel& aModelOut)
{
	aModelOut.path = aModelPath;
	FILE * file = fopen(aModelPath, "r"); // "r" is important, 
	if (file == NULL) {
		DL_ASSERT("Impossible to open file: %s\n", aModelPath);
		return false;
	}

	while (true)
	{
		char lineHeader[128]; // this assumes that the first word of the line is less then 128 chars
		// read the first word of the line
		int res = fscanf(file, "%s", lineHeader);
		if (res == EOF)
			break; // EOF = End Of File. Quit the loop.

		if (strcmp(lineHeader, "v") == 0)
		{
			CU::Vector3f vertex;
			fscanf(file, "%f %f %f\n", &vertex.x, &vertex.y, &vertex.z);
			vertex *= OBJ_SCALE;
			//vertex.y += 500.f;
			vertex.z *= -1.0f;
			aModelOut.VerticesList.Add(vertex);
		}
		else if (strcmp(lineHeader, "vt") == 0)
		{
			CU::Vector2f uv;
			fscanf(file, "%f %f\n", &uv.x, &uv.y);
			aModelOut.UVList.Add(uv);
		}
		else if (strcmp(lineHeader, "vn") == 0)
		{
			CU::Vector3f normal;
			fscanf(file, "%f %f %f\n", &normal.x, &normal.y, &normal.z);
			aModelOut.NormalsList.Add(normal);
		}

		else if (strcmp(lineHeader, "f") == 0)
		{
			unsigned int vertexIndex[3];// , uvIndex[3], normalIndex[3];
			int matches = fscanf(file, "%d %d %d "/* %d/%d/%d %d/%d/%d\n"*/,
				&vertexIndex[0], /*&uvIndex[0], &normalIndex[0],*/
				&vertexIndex[1], /*&uvIndex[1], &normalIndex[1],*/
				&vertexIndex[2] /*&uvIndex[2], &normalIndex[2]*/);

			if (matches != 3)
			{
				DL_ASSERT("This have polygons with more than three vertices. \nThis is not supported by loader at this moment. Triangulate the mesh.");
				return false;
			}
			
			SObjLoaderVertex first(vertexIndex[0] - 1, 0, 0);//uvIndex[0] - 1, normalIndex[0] - 1);
			SObjLoaderVertex second(vertexIndex[1] - 1, 0, 0);// uvIndex[1] - 1, normalIndex[1] - 1);
			SObjLoaderVertex third(vertexIndex[2] - 1, 0, 0);// uvIndex[2] - 1, normalIndex[2] - 1);

			aModelOut.FaceList.Add(SObjLoaderFace(first, second, third));
		}
	}
	return true;
}

#pragma warning (pop)


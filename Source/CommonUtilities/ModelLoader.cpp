#include "stdafx.h"
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "vector.h"
#include "PointCloud.h"
#include "Voxel.h"
#include <tga2d\math\color.h>

std::string GetWord(const std::string &aStr, int& aStartIndex) // returns the first word in the string and increments the index to the start of the next word
{
	std::string returnString;
	while (aStr[aStartIndex] != 0 && aStr[aStartIndex] != '\n' && aStr[aStartIndex] != ' ')//checks the string wordending character
	{
		returnString += aStr[aStartIndex];
		++aStartIndex;
	}
	if (aStr[aStartIndex] != 0)
	{
		++aStartIndex;
	}
	return returnString;
}

void LoadModel(const std::string &aFile, PointCloud& aModel) // loads a .ply model blenders obj format
{
	std::fstream modelFile(aFile);
	const int maxSizeOfLine = 512;//max number of characters per line
	char lineBuffer[maxSizeOfLine];
	int nrOfVertexes = 0;

	struct TypeDesc
	{
		std::string myName;
		std::string myType;
	};

	std::vector<TypeDesc> properties;

	std::map<std::string, std::vector<float> > floatProperties;//lookup table for all the float values in the modeldata
	std::map<std::string, std::vector<unsigned char> > uCharProperties;//lookup table for all the unsigned char values in the modeldata

	//this reads the description of the modeldata
	while (modelFile.getline(lineBuffer, maxSizeOfLine))
	{
		int startIndex = 0;
		std::string type = GetWord(lineBuffer, startIndex);
		if (strcmp(type.c_str(), "element") == 0)
		{
			std::string elementType = GetWord(lineBuffer, startIndex);
			if (strcmp(elementType.c_str(), "vertex") == 0)
			{
				std::string vertexCount = GetWord(lineBuffer, startIndex);
				nrOfVertexes = atoi(vertexCount.c_str());
			}
		}
		if (strcmp(type.c_str(), "property") == 0)
		{
			//finds out how the data is arranged in the file
			TypeDesc desc;
			desc.myType = GetWord(lineBuffer, startIndex);
			desc.myName = GetWord(lineBuffer, startIndex);
			if (strcmp(desc.myType.c_str(), "float") == 0)
			{
				floatProperties[desc.myName] = std::vector<float>();
				properties.push_back(desc);
			}
			else if (strcmp(desc.myType.c_str(), "uchar") == 0)
			{
				uCharProperties[desc.myName] = std::vector<unsigned char>();
				properties.push_back(desc);
			}

		}
		//end_header string ends the description part of the file
		if (strcmp(lineBuffer, "end_header") == 0)
		{
			break;
		}
	}

	//reads alla the vertexdata that has the layout found in the properties list
	for (int i = 0; i < nrOfVertexes; ++i)
	{
		//One line contains all the data for a single vertexpoint
		modelFile.getline(lineBuffer, maxSizeOfLine);
		int index = 0;
		for (size_t j = 0; j < properties.size(); ++j)
		{
			//reads every value in the order found in the header of the file
			std::string currentNumber = GetWord(lineBuffer, index);
			if (strcmp(properties[j].myType.c_str(), "float") == 0)
			{
				floatProperties[properties[j].myName].push_back(atof(currentNumber.c_str()));
			}
			else if (strcmp(properties[j].myType.c_str(), "uchar") == 0)
			{
				uCharProperties[properties[j].myName].push_back(unsigned char(atoi(currentNumber.c_str())));
			}
		}
	}

	//Adds all the vertexpoints to my pointclusterobject. The color is stored as 0-1 float values this is to be able to use them later to create lighting effect
	for (int i = 0; i < nrOfVertexes; ++i)
	{
		Voxel newVoxel(Vector3<float>(floatProperties["x"][i], floatProperties["z"][i], floatProperties["y"][i])*0.5f
			, Vector3f(float(uCharProperties["red"][i]) / 255.0f, float(uCharProperties["green"][i]) / 255.0f, float(uCharProperties["blue"][i]) / 255.0f),
			Vector3<float>(floatProperties["nx"][i], -floatProperties["nz"][i], floatProperties["ny"][i]));

		aModel.AddPoint(newVoxel);
	}
}
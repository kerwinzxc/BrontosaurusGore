#include "stdafx.h"
#include "DroppedFile.h"
#include "../ModelViewer/ModelViewer.h"

DroppedFile::DroppedFile(const std::string& aFilePath)
	: myFilePath(aFilePath)
{
}

DroppedFile::~DroppedFile()
{
}

eMessageReturn DroppedFile::DoEvent(CModelViewer* aModelViewer) const
{
	if (aModelViewer != nullptr)
	{
		if (myFilePath.rfind(".fbx") != std::string::npos)
		{
#ifdef _MODEL_VIEWER

			aModelViewer->LoadModel(myFilePath);

#endif // _MODEL_VIEWER
		}
		else
		{
			DL_PRINT_WARNING("file is not fbx: %s", myFilePath.c_str());
		}
	}

	return eMessageReturn::eContinue;
}

#pragma once
#include "Event.h"

class DroppedFile :	public Event
{
public:
	DroppedFile(const std::string& aFilePath);
	~DroppedFile();

	eMessageReturn DoEvent(CModelViewer* aModelViewer) const override;

private:
	std::string myFilePath;
};

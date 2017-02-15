#pragma once
#include "Event.h"

namespace CU
{
	enum class eInputMessage;
}

class InputMessagePressed : public Event
{
public:
	InputMessagePressed(const CU::eInputMessage& aKey);
	~InputMessagePressed();

private:
	const CU::eInputMessage& myInputMessage;
};


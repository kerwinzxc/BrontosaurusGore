#pragma once
#include "Event.h"
#include "../ThreadedPostmaster/Message.h"

class DroppedFile :	public Postmaster::Message::IMessage
{
public:
	DroppedFile(const std::string& aFilePath);
	~DroppedFile();

	//eMessageReturn DoEvent(CModelViewer* aModelViewer) const override;

	eMessageReturn DoEvent(::Postmaster::ISubscriber& aSubscriber) const override;
	const std::string& GetFilePath() const;

	inline IMessage* Copy() override;
private:
	std::string myFilePath;
};

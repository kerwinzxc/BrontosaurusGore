#include "stdafx.h"
#include "DroppedFile.h"
#include "ThreadedPostmaster/Subscriber.h"

DroppedFile::DroppedFile(const std::string& aFilePath) : IMessage(eMessageType::eDroppedFile)
, myFilePath(aFilePath)
{
}

DroppedFile::~DroppedFile()
{
}

eMessageReturn DroppedFile::DoEvent(::Postmaster::ISubscriber& aSubscriber) const
{
	return aSubscriber.DoEvent(*this);
}

const std::string& DroppedFile::GetFilePath() const
{
	return myFilePath;
}

Postmaster::Message::IMessage* DroppedFile::Copy()
{
	return new DroppedFile(*this);
}

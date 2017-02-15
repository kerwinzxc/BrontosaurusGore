#include "stdafx.h"
#include "CollisionRenderer.h"
#include "CollisionRenderMessages.h"

CCollisionRenderer::CCollisionRenderer()
	: myRenderQueue(32u)
	, myVertexBuffer(1024u)
{
}

CCollisionRenderer::~CCollisionRenderer()
{
	myRenderQueue.DeleteAll();
}

void CCollisionRenderer::AddToRender(Collision::IRenderCommand* aRenderMessage)
{
	if (aRenderMessage != nullptr)
	{
		myRenderQueue.Add(aRenderMessage);
	}
}

const CU::GrowingArray<char, unsigned int, false>& CCollisionRenderer::PrepareBuffer()
{
	myVertexBuffer.RemoveAll();

	for (Collision::IRenderCommand* renderMessage : myRenderQueue)
	{
		if (renderMessage == nullptr) continue;

		renderMessage->DoRenderCall(myVertexBuffer);
	}

	myRenderQueue.DeleteAll();

	return myVertexBuffer;
}

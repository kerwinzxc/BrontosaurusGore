#include "stdafx.h"
#include "CollisionRenderMessages.h"
#include "../CommonUtilities/vector4.h"

#include "../Components/GameObject.h"


// NEJ
static float WorldGroundY = 0.f;

Collision::IRenderCommand::IRenderCommand()
{
}

Collision::IRenderCommand::~IRenderCommand()
{
}

Collision::CRenderCircle::CRenderCircle(const Intersection::SCircle& aCircle)
	: myCircle(aCircle)
{
}

Collision::CRenderCircle::~CRenderCircle()
{
}

void Collision::CRenderCircle::DoRenderCall(CU::GrowingArray<char, unsigned int, false>& aVertexBuffer)
{
	const unsigned short NumberOfPoints(16u);
	const unsigned short NumberOfVertices(NumberOfPoints * 2u);
	const float Tau = 3.141592f * 2.f;
	const float Incrementation = Tau / static_cast<float>(NumberOfPoints);

	static CU::GrowingArray<CU::Vector4f, unsigned short, false> circleVertexBuffer(NumberOfVertices);

	for (float angle = 0.f; angle < Tau; angle += Incrementation)
	{
		const CU::Vector4f centerPointOnCircle(myCircle.myCenterPosition.x, WorldGroundY, myCircle.myCenterPosition.y, 1.f);

		CU::Vector4f firstPointOnCircle = centerPointOnCircle;
		firstPointOnCircle.x += myCircle.myRadius * std::cosf(angle);
		firstPointOnCircle.z += myCircle.myRadius * std::sinf(angle);

		circleVertexBuffer.Add(firstPointOnCircle);

		CU::Vector4f secondPointOnCircle = centerPointOnCircle;
		secondPointOnCircle.x += myCircle.myRadius * std::cosf(angle + Incrementation);
		secondPointOnCircle.z += myCircle.myRadius * std::sinf(angle + Incrementation);

		circleVertexBuffer.Add(secondPointOnCircle);
	}

	aVertexBuffer.AddChunk(circleVertexBuffer.AsVoidPointer(), circleVertexBuffer.ByteSize());
	circleVertexBuffer.RemoveAll();
}

Collision::CRenderTriangle::CRenderTriangle(const Intersection::STriangle* aTriangle)
	: myTriangle(aTriangle)
{
}

Collision::CRenderTriangle::~CRenderTriangle()
{
}

void Collision::CRenderTriangle::DoRenderCall(CU::GrowingArray<char, unsigned int, false>& aVertexBuffer)
{
	CU::Vector4f triangleVertexBuffer[6u] =
	{
		CU::Vector4f(myTriangle->myPoints[0].x, WorldGroundY, myTriangle->myPoints[0].y, 1.f), CU::Vector4f(myTriangle->myPoints[1].x, WorldGroundY, myTriangle->myPoints[1].y, 1.f),
		CU::Vector4f(myTriangle->myPoints[1].x, WorldGroundY, myTriangle->myPoints[1].y, 1.f), CU::Vector4f(myTriangle->myPoints[2].x, WorldGroundY, myTriangle->myPoints[2].y, 1.f),
		CU::Vector4f(myTriangle->myPoints[2].x, WorldGroundY, myTriangle->myPoints[2].y, 1.f), CU::Vector4f(myTriangle->myPoints[0].x, WorldGroundY, myTriangle->myPoints[0].y, 1.f)
	};

	aVertexBuffer.AddChunk(triangleVertexBuffer, sizeof(triangleVertexBuffer));
}

Collision::CRenderPoint::CRenderPoint(const Intersection::SPoint& aPoint)
	: myPoint(aPoint)
{
}

Collision::CRenderPoint::~CRenderPoint()
{
}

void Collision::CRenderPoint::DoRenderCall(CU::GrowingArray<char, unsigned int, false>& aVertexBuffer)
{
	static const float CrossRadius = 30.f;
	CU::Vector4f pointCrossVertexBuffer[4] =
	{
		CU::Vector4f(myPoint.myPosition.x - CrossRadius, WorldGroundY, myPoint.myPosition.y - CrossRadius, 1.f),
		CU::Vector4f(myPoint.myPosition.x + CrossRadius, WorldGroundY, myPoint.myPosition.y + CrossRadius, 1.f),
		CU::Vector4f(myPoint.myPosition.x + CrossRadius, WorldGroundY, myPoint.myPosition.y - CrossRadius, 1.f),
		CU::Vector4f(myPoint.myPosition.x - CrossRadius, WorldGroundY, myPoint.myPosition.y + CrossRadius, 1.f)
	};

	aVertexBuffer.AddChunk(pointCrossVertexBuffer, sizeof(pointCrossVertexBuffer));
}

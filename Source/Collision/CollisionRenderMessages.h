#pragma once

namespace Intersection
{
	struct SCircle;
	struct STriangle;
}

namespace Collision
{
	class IRenderCommand
	{
	public:
		IRenderCommand();
		virtual ~IRenderCommand();
		virtual void DoRenderCall(CU::GrowingArray<char, unsigned int, false>& aVertexBuffer) = 0;
	};

	class CRenderCircle : public IRenderCommand
	{
	public:
		CRenderCircle(const Intersection::SCircle& aCircle);
		~CRenderCircle();
		void DoRenderCall(CU::GrowingArray<char, unsigned int, false>& aVertexBuffer) override;

	private:
		const Intersection::SCircle& myCircle;
	};

	class CRenderTriangle : public IRenderCommand
	{
	public:
		CRenderTriangle(const Intersection::STriangle* aCircle);
		~CRenderTriangle();
		void DoRenderCall(CU::GrowingArray<char, unsigned int, false>& aVertexBuffer) override;

	private:
		const Intersection::STriangle* myTriangle;
	};

	class CRenderPoint : public IRenderCommand
	{
	public:
		CRenderPoint(const Intersection::SPoint& aPoint);
		~CRenderPoint();
		void DoRenderCall(CU::GrowingArray<char, unsigned int, false>& aVertexBuffer) override;

	private:
		const Intersection::SPoint& myPoint;
	};
}
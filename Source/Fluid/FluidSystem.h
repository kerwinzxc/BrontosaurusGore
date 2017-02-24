#pragma once
#include "../CommonUtilities/matrix44.h"
#include "../CommonUtilities/GrowingArray.h"
#include "FluidParticle.h"
#include "../CommonUtilities/CUTime.h"
#include "../Container/LocklessQueue.h"

namespace Simulation
{
	class CFluidSystem
	{
	public:
		CFluidSystem();
		~CFluidSystem();

		void Run();

		void Update(const CU::Time& aDt);
		void Render() const;

		void AddParticles(const CU::GrowingArray<SFluidParticle, unsigned, false>& aParticle);
		void SetBounds(const CU::Vector3f& aMin, const CU::Vector3f& aMax);
	protected:
		void UpdateLastParticles();
		const CU::GrowingArray<SFluidParticle, unsigned int, false> GetLastParticles() const;
		CU::GrowingArray<SFluidParticle, unsigned int, false> myParticles;
		CU::GrowingArray<SFluidParticle, unsigned int, false> myLastParticles;
		Container::CLocklessQueue<CU::GrowingArray<SFluidParticle, unsigned int, false>> myParticlesToAdd;
		bool myIsRunning;
		CU::Vector3f myMin;
		CU::Vector3f myMax;
	};
}


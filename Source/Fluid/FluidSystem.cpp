#include "FluidSystem.h"
#include <thread>
#include "../CommonUtilities/TimerManager.h"

Simulation::CFluidSystem::CFluidSystem(): myIsRunning(false)
{
}

Simulation::CFluidSystem::~CFluidSystem()
{
}

void Simulation::CFluidSystem::Run()
{
	CU::TimerManager timerManager;
	timerManager.CreateTimer();
	timerManager.UpdateTimers();
	while (myIsRunning == true)
	{
		timerManager.UpdateTimers();
		Update(timerManager.GetTimer(0).GetDeltaTime());
		std::this_thread::yield();
	}
}

void Simulation::CFluidSystem::Update(const CU::Time& aDt)
{
	while (myParticlesToAdd.IsEmpty() == false)
	{
		
	}
}

void Simulation::CFluidSystem::Render() const
{
	CU::GrowingArray<SFluidParticle, unsigned, false> renderParticles = GetLastParticles();
}

void Simulation::CFluidSystem::AddParticles(const CU::GrowingArray<SFluidParticle, unsigned, false>& aParticle)
{
	myParticlesToAdd.Push(aParticle);
}

void Simulation::CFluidSystem::UpdateLastParticles()
{
}

const CU::GrowingArray<SFluidParticle, unsigned, false> Simulation::CFluidSystem::GetLastParticles() const
{
	return myLastParticles;
}

void Simulation::CFluidSystem::SetBounds(const CU::Vector3f& aMin, const CU::Vector3f& aMax)
{
	myMin = aMin;
	myMax = aMax;
}

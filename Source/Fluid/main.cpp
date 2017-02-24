
#include "FluidSystem.h"

int main(int argc, char* argv[])
{
	Simulation::CFluidSystem fluidSystem;
	fluidSystem.SetBounds(CU::Vector3f(-10, -10, -10), CU::Vector3f(10, 10, 10));
	CU::GrowingArray<SFluidParticle, unsigned, false> particles;
	particles.Init(100);
	for(int i = 0; i < 10000; ++i)
	{
		SFluidParticle particle;
		particle.position = CU::Vector3f(static_cast<float>(rand() % 1000) / 100 - 5, static_cast<float>(rand() % 1000) / 100 - 5, static_cast<float>(rand() % 1000) / 100 - 5);
		particles.Add(particle);
	}
	fluidSystem.AddParticles(particles);

	return 0;
}

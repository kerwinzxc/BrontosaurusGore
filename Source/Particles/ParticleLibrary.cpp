#include "ParticleLibrary.h"
#include "JsonValue.h"
#include "DL_Debug.h"


Particles::CParticleLibrary::CParticleLibrary()
{
}


Particles::CParticleLibrary::~CParticleLibrary()
{
}

void Particles::CParticleLibrary::Load(const std::string& aPath)
{
	/*CU::CJsonValue levelsFile;
	std::string errorString = levelsFile.Parse(aPath);
	if (!errorString.empty()) DL_MESSAGE_BOX(errorString.c_str());

	CU::CJsonValue systemsArray = levelsFile["systems"];

	for(int i = 0; i < systemsArray.Size(); ++i)
	{
		LoadSystem(systemsArray[i]);
	}*/
}

Particles::CParticleLibrary::ParticleId Particles::CParticleLibrary::AddRef(const std::string& aSystemName)
{
	std::map<std::basic_string<char>, int>::iterator it = myStringIdMap.find(aSystemName);

	if (it == myStringIdMap.end())
	{
		DL_ASSERT("Particly emitter with name \"%s\" doesn't exist.", aSystemName.c_str());
	}

	AddRef(it->second);
	return it->second;
}

void Particles::CParticleLibrary::AddRef(const ParticleId anId)
{
	std::map<int, CParticleEmitter>::iterator it = myIdEmitterMap.find(anId);

	if(it == myIdEmitterMap.end())
	{
		DL_ASSERT("Particly emitter with id %d doesn't exist.", anId);
	}

	it->second.AddRef();
}

void Particles::CParticleLibrary::RemoveRef(const std::string& aSystemName)
{
	std::map<std::basic_string<char>, int>::iterator it = myStringIdMap.find(aSystemName);

	if (it == myStringIdMap.end())
	{
		DL_ASSERT("Particly emitter with name \"%s\" doesn't exist.", aSystemName.c_str());
	}

	RemoveRef(it->second);
}

void Particles::CParticleLibrary::RemoveRef(const ParticleId anId)
{
	std::map<int, CParticleEmitter>::iterator it = myIdEmitterMap.find(anId);

	if (it == myIdEmitterMap.end())
	{
		DL_ASSERT("Particly emitter with id %d doesn't exist.", anId);
	}

	it->second.RemoveRef();
}

void Particles::CParticleLibrary::AddSystemRef(const std::string& aName, const ParticleId aId)
{
	std::map<std::basic_string<char>, int>::iterator stringIt = myStringIdMap.find(aName);
	std::map<int, CParticleEmitter>::iterator idIt = myIdEmitterMap.find(aId);
	if(stringIt != myStringIdMap.end())
	{
		DL_ASSERT("Particly emitter with name \"%s\" already exists.", aName.c_str());
	}

	if(idIt != myIdEmitterMap.end())
	{
		DL_ASSERT("Particly emitter with id %d already exists.", aId);
	}

	myStringIdMap[aName] = aId;
}

void Particles::CParticleLibrary::LoadSystem(const CU::CJsonValue& aJsonValue)
{
	const std::string name = aJsonValue["name"].GetString();
	const ParticleId id = aJsonValue["id"].GetInt();
	AddSystemRef(name, id);

	CParticleEmitter emitter(aJsonValue);

	myIdEmitterMap[id] = emitter;
}

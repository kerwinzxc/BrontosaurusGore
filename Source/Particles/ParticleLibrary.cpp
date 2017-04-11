#include "ParticleLibrary.h"
#include "JsonValue.h"
#include "DL_Debug.h"

const Particles::ParticleEmitterID Particles::CParticleLibrary::FoundNone = -1;

Particles::CParticleLibrary::CParticleLibrary()
{
}


Particles::CParticleLibrary::~CParticleLibrary()
{
}

void Particles::CParticleLibrary::Load(const std::string& aPath)
{
	CU::CJsonValue particlesFile;
	std::string errorString = particlesFile.Parse(aPath);
	if (!errorString.empty())
	{
		DL_MESSAGE_BOX(errorString.c_str());
		return;
	}

#ifndef _RETAIL_BUILD
	if(myFile.empty())
	{
		WatchFile(aPath);
	}
	myFile = aPath;
#endif

	CU::CJsonValue systemsArray = particlesFile["systems"];

	for(int i = 0; i < systemsArray.Size(); ++i)
	{
		LoadSystem(systemsArray[i]);
	}
}

Particles::ParticleEmitterID Particles::CParticleLibrary::AddRef(const std::string& aSystemName)
{
	std::map<std::basic_string<char>, int>::iterator it = myStringIdMap.find(aSystemName);

	if (it == myStringIdMap.end())
	{
		DL_ASSERT("Particly emitter with name \"%s\" doesn't exist.", aSystemName.c_str());
	}

	AddRef(it->second);
	return it->second;
}

void Particles::CParticleLibrary::AddRef(const Particles::ParticleEmitterID anId)
{
	std::map<int, CParticleEmitter*>::iterator it = myIdEmitterMap.find(anId);

	if(it == myIdEmitterMap.end())
	{
		DL_ASSERT("Particly emitter with id %d doesn't exist.", anId);
	}

	it->second->AddRef();
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

void Particles::CParticleLibrary::RemoveRef(const Particles::ParticleEmitterID anId)
{
	std::map<int, CParticleEmitter*>::iterator it = myIdEmitterMap.find(anId);

	if (it == myIdEmitterMap.end())
	{
		DL_ASSERT("Particly emitter with id %d doesn't exist.", anId);
	}

	it->second->RemoveRef();
}

Particles::ParticleEmitterID Particles::CParticleLibrary::GetSystem(const std::string& aSystemId)
{
	std::map<std::basic_string<char>, int>::iterator id = myStringIdMap.find(aSystemId);

	if(id == myStringIdMap.end())
	{
		return FoundNone;
	}

	return id->second;
}

CParticleEmitter * Particles::CParticleLibrary::GetSystemP(ParticleEmitterID aParticleEmitter)
{
	std::map<int, CParticleEmitter*>::iterator it = myIdEmitterMap.find(aParticleEmitter);
	if (it == myIdEmitterMap.end())
	{
		return nullptr;
	}
	return it->second;
}

void Particles::CParticleLibrary::AddSystemRef(const std::string& aName, const Particles::ParticleEmitterID aId)
{
	std::map<std::basic_string<char>, int>::iterator stringIt = myStringIdMap.find(aName);
	std::map<int, CParticleEmitter*>::iterator idIt = myIdEmitterMap.find(aId);
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
	const Particles::ParticleEmitterID id = aJsonValue["id"].GetInt();
	AddSystemRef(name, id);

	CParticleEmitter* emitter = new CParticleEmitter(aJsonValue);

	myIdEmitterMap[id] = emitter;
}

#ifndef _RETAIL_BUILD
void Particles::CParticleLibrary::Reload()
{
	std::map<int, CParticleEmitter*>::iterator it;
	for(it = myIdEmitterMap.begin(); it != myIdEmitterMap.end(); ++it)
	{
		delete it->second;
	}
	myIdEmitterMap.clear(); 
	myStringIdMap.clear();

	Load(myFile);
}

void Particles::CParticleLibrary::WatchFile(const std::string& aPath)
{
	const std::wstring watchFile(aPath.begin(), aPath.end());
	std::function<void(const std::wstring&)> callback = [this](const std::wstring& something) {Reload(); };
	myFileWatcher.WatchFileChange(watchFile, callback);
}
#endif
#include "Mono_Manager.h"

#include <iostream>

void MonoManager::InitMono()
{
    mono_set_assemblies_path("../mono/lib/4.5");

    MonoDomain* rootDomain = mono_jit_init("MyScriptRuntime");
	if (rootDomain == nullptr)
	{
		// Maybe log some error here
		std::cout << "MonoDomain has not been initialised correctly." << std::endl;
		return;
	}

	// Store the root domain pointer
	monoRootDomain = rootDomain;

	char appDomainName[] = "MyAppDomain";
	monoAppDomain = mono_domain_create_appdomain(appDomainName, nullptr);
	mono_domain_set(monoAppDomain, true);

	AddInternalCalls();
}

void MonoManager::ShutDownMono()
{
	mono_domain_set(mono_get_root_domain(), false);

	mono_domain_unload(monoAppDomain);
	monoAppDomain = nullptr;

	mono_jit_cleanup(monoRootDomain);
	monoRootDomain = nullptr;
}

MonoAssembly* MonoManager::LoadAssembly(std::string assemblyPath)
{
	return nullptr;
}

MonoClass* MonoManager::GetClass(MonoAssembly* assembly, std::string className)
{
	return nullptr;
}

MonoObject* MonoManager::InstantiateClass(std::string assemblyPath, std::string className)
{
	return nullptr;
}

void MonoManager::CallCsharp(MonoObject* objectInstance)
{
}

void MonoManager::AddInternalCalls()
{
	std::cout << "Adding internal calls" << std::endl;
}

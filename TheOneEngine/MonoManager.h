#pragma once

#include "mono\jit\jit.h"
#include "mono\metadata\assembly.h"
#include "mono\metadata\attrdefs.h"

#include <string>

class MonoManager
{
public:
    MonoManager() {}
    ~MonoManager() {}

    void InitMono();
    void ShutDownMono();

    MonoAssembly* LoadAssembly(std::string assemblyPath);
    MonoClass* GetClass(MonoAssembly* assembly, std::string className);
    MonoObject* InstantiateClass(std::string assemblyPath, std::string className);

    void CallCsharp(MonoObject* objectInstance);
    void AddInternalCalls();

private:
    MonoDomain* monoRootDomain = nullptr;
    MonoDomain* monoAppDomain = nullptr;
};


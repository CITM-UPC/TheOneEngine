#pragma once

#include "mono\jit\jit.h"
#include "mono\metadata\assembly.h"
#include "mono\metadata\attrdefs.h"

#include <string>

class MonoManager
{
    struct MonoManagerData
    {
        MonoDomain* monoRootDomain = nullptr;
        MonoDomain* monoAppDomain = nullptr;

        MonoAssembly* mainAssembly = nullptr;

        unsigned long currentUUID = -1;
    };

private:

    static MonoManagerData monoData;

public:
    MonoManager() {}
    ~MonoManager() {}

    void InitMono();
    void ShutDownMono();

    static MonoObject* InstantiateClass(const char* className, unsigned long = -1);

    void CallScriptFunction(MonoObject* monoBehaviourInstance, std::string functionToCall);
    void CallScriptFunction(MonoObject* monoBehaviourInstance, std::string functionToCall, void** params, int parameterCount);

    //Testing function only. Prints all the classes loaded in the given assembly onto the debug console (not LOG).
    void PrintAssemblyClasses(const std::string& assemblyPath);

private:
    char* ReadBytes(const std::string& filepath, uint32_t* outSize);
    MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath);
    static MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);
};


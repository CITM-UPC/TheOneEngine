#pragma once
#include "GameObject.h"

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

        GameObject* currentGameObjectPtr = nullptr;
    };

public:
    GameObject* bulletGO;

private:

    static MonoManagerData monoData;

public:
    MonoManager() {}
    ~MonoManager() {}

    void InitMono();
    void ShutDownMono();

    static GameObject* GetCurrentGameObjectPtr() { return monoData.currentGameObjectPtr; }
    static MonoObject* InstantiateClass(const char* className, GameObject* containerGOptr);

    static void CallScriptFunction(MonoObject* monoBehaviourInstance, std::string functionToCall);
    static void CallScriptFunction(MonoObject* monoBehaviourInstance, std::string functionToCall, void** params, int parameterCount);

    static bool IsClassInMainAssembly(const char* className);

private:
    char* ReadBytes(const std::string& filepath, uint32_t* outSize);
    MonoAssembly* LoadCSharpAssembly(const std::string& assemblyPath);
    static MonoClass* GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className);
};


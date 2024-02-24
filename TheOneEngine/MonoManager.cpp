#include "MonoManager.h"

#include <iostream>
#include <fstream>

#ifdef _DEBUG
const std::string mainAssemblyPath = "../TheOneScriptingEngine/bin/Debug/TheOneScriptingEngine.dll";
#else
const std::string mainAssemblyPath = "../TheOneScriptingEngine/bin/Release/TheOneScriptingEngine.dll";
#endif

void MonoManager::InitMono()
{
    mono_set_assemblies_path("../mono/lib/4.5");

    MonoDomain* rootDomain = mono_jit_init("MyScriptRuntime");
	if (rootDomain == nullptr)
	{
		std::cout << "MonoDomain has not been initialised correctly." << std::endl;
		return;
	}

	// Store the root domain pointer
	monoRootDomain = rootDomain;

	char appDomainName[] = "MyAppDomain";
	monoAppDomain = mono_domain_create_appdomain(appDomainName, nullptr);
	mono_domain_set(monoAppDomain, true);

    mainAssembly = LoadCSharpAssembly(mainAssemblyPath);
}

void MonoManager::ShutDownMono()
{
	mono_domain_set(mono_get_root_domain(), false);

	mono_domain_unload(monoAppDomain);
	monoAppDomain = nullptr;

	mono_jit_cleanup(monoRootDomain);
	monoRootDomain = nullptr;
}

MonoObject* MonoManager::InstantiateClass(const std::string& assemblyPath, const char* namespaceName, const char* className)
{
    // Get a reference to the class we want to instantiate
    MonoClass* classToInstantiate = GetClassInAssembly(mainAssembly, namespaceName, className);

    if (classToInstantiate == nullptr)
    {
        std::cout << "Could not find a C# class named " << className << std::endl;
        return nullptr;
    }

    // Allocate an instance of our class
    MonoObject* classInstance = mono_object_new(monoAppDomain, classToInstantiate);

    if (classInstance == nullptr)
    {
        std::cout << "Could not create instance of C# class " << className << std::endl;
        return nullptr;
    }

    // Call the parameterless (default) constructor
    mono_runtime_object_init(classInstance);
    std::cout << "Instance of " << className << " created and initialized." << std::endl;

    return classInstance;
}

void MonoManager::CallScriptFunction(MonoObject* monoBehaviourInstance, std::string functionToCall)
{
    // Get the MonoClass pointer from the instance
    MonoClass* instanceClass = mono_object_get_class(monoBehaviourInstance);

    // Get a reference to the method in the class
    MonoMethod* method = mono_class_get_method_from_name(instanceClass, functionToCall.c_str(), 0);

    if (method == nullptr)
    {
        std::cout << "Could not find method " << functionToCall << std::endl;
        return;
    }

    // Call the C# method on the objectInstance instance, and get any potential exceptions
    MonoObject* exception = nullptr;
    mono_runtime_invoke(method, monoBehaviourInstance, nullptr, &exception);

    //Handle the exception
    if (exception != nullptr)
    {
        std::cout << "Exception occurred" << std::endl;
        return;
    }
}

void MonoManager::CallScriptFunction(MonoObject* monoBehaviourInstance, std::string functionToCall, void** params, int parameterCount)
{
    // Get the MonoClass pointer from the instance
    MonoClass* instanceClass = mono_object_get_class(monoBehaviourInstance);

    // Get a reference to the method in the class
    MonoMethod* method = mono_class_get_method_from_name(instanceClass, functionToCall.c_str(), parameterCount);

    if (method == nullptr)
    {
        std::cout << "Could not find method " << functionToCall << std::endl;
        return;
    }

    // Call the C# method on the objectInstance instance, and get any potential exceptions
    MonoObject* exception = nullptr;
    mono_runtime_invoke(method, monoBehaviourInstance, params, &exception);

    //Handle the exception
    if (exception != nullptr)
    {
        std::cout << "Exception occurred" << std::endl;
        return;
    }
}

void MonoManager::PrintAssemblyClasses(const std::string& assemblyPath)
{
    MonoImage* image = mono_assembly_get_image(mainAssembly);
    const MonoTableInfo* typeDefinitionsTable = mono_image_get_table_info(image, MONO_TABLE_TYPEDEF);
    int32_t numTypes = mono_table_info_get_rows(typeDefinitionsTable);

    for (int32_t i = 0; i < numTypes; i++)
    {
        uint32_t cols[MONO_TYPEDEF_SIZE];
        mono_metadata_decode_row(typeDefinitionsTable, i, cols, MONO_TYPEDEF_SIZE);

        const char* nameSpace = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAMESPACE]);
        const char* name = mono_metadata_string_heap(image, cols[MONO_TYPEDEF_NAME]);

        printf("%s.%s\n", nameSpace, name);
    }
}

char* MonoManager::ReadBytes(const std::string& filepath, uint32_t* outSize)
{
    std::ifstream stream(filepath, std::ios::binary | std::ios::ate);

    if (!stream)
    {
        // Failed to open the file
        return nullptr;
    }

    std::streampos end = stream.tellg();
    stream.seekg(0, std::ios::beg);
    uint32_t size = end - stream.tellg();

    if (size == 0)
    {
        // File is empty
        return nullptr;
    }

    char* buffer = new char[size];
    stream.read((char*)buffer, size);
    stream.close();

    *outSize = size;
    return buffer;
}

MonoAssembly* MonoManager::LoadCSharpAssembly(const std::string& assemblyPath)
{
    uint32_t fileSize = 0;
    char* fileData = ReadBytes(assemblyPath, &fileSize);

    // NOTE: We can't use this image for anything other than loading the assembly because this image doesn't have a reference to the assembly
    MonoImageOpenStatus status;
    MonoImage* image = mono_image_open_from_data_full(fileData, fileSize, 1, &status, 0);

    if (status != MONO_IMAGE_OK)
    {
        const char* errorMessage = mono_image_strerror(status);
        std::cout << "Mono image had an error when loading assembly. Error: " << errorMessage << std::endl;
        return nullptr;
    }

    MonoAssembly* assembly = mono_assembly_load_from_full(image, assemblyPath.c_str(), &status, 0);
    mono_image_close(image);

    // Don't forget to free the file data
    delete[] fileData;

    return assembly;
}

MonoClass* MonoManager::GetClassInAssembly(MonoAssembly* assembly, const char* namespaceName, const char* className)
{
    MonoImage* image = mono_assembly_get_image(assembly);
    MonoClass* cSharpClass = mono_class_from_name(image, namespaceName, className);

    return cSharpClass;
}

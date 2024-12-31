// Main program: main.c
#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

#include "testlib_api.h"

void EmitComponentInfo(ComponentInfo_t* info) {
    printf("Class Info - Name: %s, ID: %d\n", info->className, info->classId);
}

void EmitInterfaceInfo(InterfaceInfo_t* info) {
    printf("Interface Info - Name: %s, ID: %d\n", info->name, info->interfaceId);
}

void EmitTypeInfo(BasicTypeInfo_t* info) {
    printf("Interface Info - Name: %s, ID: %d\n", info->name, info->typeID);
}

int main() {
    // Load the shared library
    void *libHandle = dlopen("./testlib.so", RTLD_LAZY);
    if (!libHandle) {
        fprintf(stderr, "Failed to load library: %s\n", dlerror());
        return 1;
    }

    // Resolve the EmitApi function
    LibraryApi (*EmitApi)(void) = dlsym(libHandle, "EmitApi");
    if (!EmitApi) {
        fprintf(stderr, "Failed to locate EmitApi: %s\n", dlerror());
        dlclose(libHandle);
        return 1;
    }

    // Resolve the SetupMainApi function
    void (*SetupMainApi)(const MainApi *) = dlsym(libHandle, "SetupMainApi");
    if (!SetupMainApi) {
        fprintf(stderr, "Failed to locate SetupMainApi: %s\n", dlerror());
        dlclose(libHandle);
        return 1;
    }

    // Define MainApi
    MainApi mainApi = {
        .EmitComponentInfo = EmitComponentInfo,
        .EmitInterfaceInfo = EmitInterfaceInfo,
        .EmitTypeInfo = EmitTypeInfo,
        .RegisterCallback = registerCallback,
        .RegisterInterface = registerInterface
    };

    // Load the shared library's API
    LibraryApi libraryApi = EmitApi();

    // Set up the shared library with MainApi
    SetupMainApi(&mainApi);

    // Call shared library functions
    libraryApi.EnumerateTypes(0);
    libraryApi.EnumerateComponents(0);

    ObjectId foo = allocateObject(0);
    ObjectId foo2 = allocateObject(0);
    ObjectId foo3 = allocateObject(0);

    Component_t component = libraryApi.AllocComponent(1);
    Component_t component2 = libraryApi.AllocComponent(1);

    addComponent(foo, component);
    addComponent(foo3, component2);

    InterfaceInfo_t* info = queryInterface(foo, "TestFuncTable");
    if (info)
    {
        printf("Calling TestFuncTable\n");
        TestFuncTableStruct_t* testInterfaceTable = (TestFuncTableStruct_t*)info->functionTable;
        testInterfaceTable->testFunc(&component);
        testInterfaceTable->testOtherFunc(&component, 10);
    }
    else
    {
        printf("No TestFuncTable\n");
    }

    printf("Exec'ing tick\n");

    executeGlobalCallbacks(CALLBACK_TICK);

    freeObject(foo);
    freeObject(foo2);
    freeObject(foo3);

    libraryApi.FreeComponent(component);
    libraryApi.FreeComponent(component2);

    // Close the shared library
    dlclose(libHandle);

    return 0;
}

// Shared library implementation: library.c
#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "component.h"
#include "testlib_api.h"

void TestComponentInit(Component_t* c);
void TestComponentFree(Component_t* c);
void TestComponentRegisterCallbacks(ObjectId o, Component_t*c);
void TestComponentOnAdd(ObjectId o, Component_t*c);
void TestComponentOnRemove(ObjectId o, Component_t*c);
void TestComponentEnumerateInterfaces(ObjectId o, Component_t*c);

ffi_type *TestTypeFFIElements[] = {
    &ffi_type_float,
    &ffi_type_float,
    NULL
};

ffi_type TestTypeFFI = {
    sizeof(TestTypeStruct_t),
    0,
    FFI_TYPE_STRUCT,
    TestTypeFFIElements
};

BasicTypeInfo_t TestType = {
    "TestType",
    &TestTypeFFI,
    0,
    1,
    NULL,
    NULL
};

typedef struct TestComponentStruct {
    const char* EngineString;
    int32_t NumHits;
    TestTypeStruct_t TestStruct;
} TestComponentStruct;

FieldInfo_t TestComponentStructFields[] = {
    {"EngineString", coffsetof(EngineString, TestComponentStruct), 1, 0, NULL, NULL},
    {"NumHits",      coffsetof(NumHits, TestComponentStruct),      1, 0, NULL, NULL},
    {"TestStruct",   coffsetof(TestStruct, TestComponentStruct),   1, 0, NULL, NULL}
};

static ComponentInfo_t testInfo1 = {
    "TestComponent",
    TestComponentStructFields,
    sizeof(TestComponentStructFields) / sizeof(TestComponentStructFields[0]),
    sizeof(TestComponentStruct),
    0,
    1,
    &TestComponentInit,
    &TestComponentFree,
    &TestComponentRegisterCallbacks,
    &TestComponentEnumerateInterfaces
};

void testFunc(Component_t* component)
{
    printf("TestFunc called\n");
}

void testOtherFunc(Component_t* component, int foo)
{
    printf("TestOtherFunc called with %i\n", foo);
}

TestFuncTableStruct_t testInterfaceTable = {
    testFunc,
    testOtherFunc
};

static InterfaceInfo_t testInterface = {
    (void**)&testInterfaceTable,
    "TestFuncTable",
    NULL,
    1,
    1
};

// Function pointers for MainApi
const MainApi *gEngine = NULL;

void TestComponentInit(Component_t* c)
{
    printf("  INIT COMPONENT [ref=%i]\n", c->ref);

    TestComponentStruct* cd = (TestComponentStruct*)c->fieldData;
    cd->NumHits = rand()%2000;
}

void TestComponentFree(Component_t* c)
{
    printf("  FREE COMPONENT [ref=%i]\n", c->ref);
}

void TestComponentOnAdd(ObjectId o, Component_t*c)
{
    printf("ONADD COMPONENT\n");

}

void TestComponentOnRemove(ObjectId o, Component_t*c)
{
    printf("ONREMOVE COMPONENT\n");
}

void TestComponentOnTick(ObjectId o, Component_t*c)
{
    printf("TICK COMPONENT [%i]\n", o);
}

void TestComponentRegisterCallbacks(ObjectId o, Component_t*c)
{
    gEngine->RegisterCallback(o, CALLBACK_ON_ADD, c->ref, TestComponentOnAdd);
    gEngine->RegisterCallback(o, CALLBACK_ON_REMOVE, c->ref, TestComponentOnRemove);
    gEngine->RegisterCallback(o, CALLBACK_TICK, c->ref, TestComponentOnTick);
}

void TestComponentEnumerateInterfaces(ObjectId o, Component_t*c)
{
    gEngine->RegisterInterface(o, &testInterface);
}

void EnumerateComponents(uint32_t startId) {
    testInfo1.classId = startId;
    gEngine->EmitComponentInfo(&testInfo1);
}

void EnumerateTypes(uint32_t startId) {
    TestType.typeID = TestType.moduleTypeID + startId;
    gEngine->EmitTypeInfo(&TestType);
}

// Implementation of AllocClass
Component_t AllocComponent(uint32_t classId) {
    printf("Allocating component with ID: %d\n", classId);

    Component_t outInfo;
    memset(&outInfo, 0, sizeof(Component_t));

    if (classId == 1) // MODULE id
    {
        outInfo.info = &testInfo1;
    }

    if (outInfo.info  == NULL)
    {
        return outInfo;
    }

    outInfo.fieldData = malloc(outInfo.info->fieldDataSize);
    memset(outInfo.fieldData, 0, outInfo.info->fieldDataSize);

    outInfo.info->initFunc(&outInfo);

    return outInfo;
}

void FreeComponent(Component_t inInfo) {
    printf("Freeing component...\n");

    if (inInfo.info->cleanupFunc)
    {
        inInfo.info->cleanupFunc(&inInfo);
    }

    if (inInfo.fieldData)
    {
        free(inInfo.fieldData);
    }

    memset(&inInfo, 0, sizeof(Component_t));
}

uint32_t RegisterType(uint32_t typeID, ffi_type* typeInfo) {
    printf("Allocating type with ID: %d\n", typeID);
    return 0;
}

LibraryApi EmitApi(void) {
    LibraryApi api = {
        .EnumerateComponents = EnumerateComponents,
        .EnumerateTypes = EnumerateTypes,
        .AllocComponent = AllocComponent,
        .FreeComponent = FreeComponent,
        .RegisterType = RegisterType,
    };
    return api;
}

void SetupMainApi(const MainApi *api) {
    gEngine = api;
}


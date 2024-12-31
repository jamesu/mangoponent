#ifndef _COMPONENT_H_
#define _COMPONENT_H_

#include <ffi.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#define coffsetof(member, type)  __builtin_offsetof (type, member)

#define MAX_INTERFACES 10
#define MAX_COMPONENTS 10
#define MAX_OBJECTS 100

typedef uint32_t ObjectId;
typedef uint32_t ComponentRefId;

// Enum for callback types
typedef enum {
    CALLBACK_ON_ADD,
    CALLBACK_ON_REMOVE,
    CALLBACK_RENDER,
    CALLBACK_TICK,
    CALLBACK_COUNT
} CallbackType;

// Forward declarations
typedef struct Object_s Object_t;
typedef struct Component_s Component_t;

// Type for callback functions
typedef void (*CallbackFunc)(ObjectId, Component_t*);

typedef struct BasicTypeInfo_s {
    const char* name;
    ffi_type* ffiInfo;
    int typeID;
    int moduleTypeID;
    void (*writeFunc)(void* field, void* inputData, bool isString);
    void (*readFunc)(void* field, void* outputData, bool isString);
} BasicTypeInfo_t;

typedef struct CallbackNode_s {
    uint32_t component;
    CallbackFunc func;
    struct CallbackNode_s * next;
} CallbackNode_t;

typedef struct FunctionInfo_s {
    const char* name;
    ffi_type* returnType;
    ffi_type* args;
    uint32_t numArgs;
} FunctionInfo_t;

typedef struct InterfaceInfo_s {
    void** functionTable;
    const char* name;
    FunctionInfo_t* ffiTable;
    int interfaceId;
    uint32_t numFuncs;
} InterfaceInfo_t;

typedef struct FieldInfo_s {
    const char* name;
    uint32_t fieldOffset;
    uint32_t fieldType;
    uint32_t arraySize;
    void (*writeFunc)(void* field, Component_t* component, void* inputData, bool isString);
    void (*readFunc)(void* field, Component_t* component, void* outputData, bool isString);
} FieldInfo_t;

typedef struct ComponentInfo_s {
    const char *className;
    FieldInfo_t* fieldInfo;
    int fieldCount;
    uint32_t fieldDataSize;
    int classId;
    int moduleClassId;

    // Baseline funcs
    void (*initFunc)(Component_t*);
    void (*cleanupFunc)(Component_t*);
    void (*registerCallbacks)(ObjectId, Component_t*);
    void (*enumerateInterfaces)(ObjectId, Component_t*);
} ComponentInfo_t;

typedef struct Component_s {
    ComponentInfo_t* info;
    void* fieldData;
    void* privateData;
    ComponentRefId ref;
} Component_t;

// Object definition
typedef struct Object_s {
    uint32_t id;
    Component_t components[MAX_COMPONENTS];
    CallbackNode_t* callbacks[CALLBACK_COUNT];
    InterfaceInfo_t* interfaces[MAX_INTERFACES];
    int componentCount;
    int interfaceCount;
} Object_t;

// Global object list
extern Object_t globalObjects[MAX_OBJECTS];

// Function to initialize an object
ObjectId allocateObject(uint32_t ident);

// Function to free an object
void freeObject(ObjectId ident);

// Function to add a component to an object
void addComponent(ObjectId ident, Component_t component);

// Function to free all components on an object
void freeComponents(Object_t* obj);

// Function to register a callback
void registerCallback(ObjectId ident, CallbackType type, uint32_t componentIdx, CallbackFunc callback);

// Function to register an interface
void registerInterface(ObjectId ident, InterfaceInfo_t* info);

// Function to query an interface
InterfaceInfo_t* queryInterface(ObjectId ident, const char* name);

// Function to execute callbacks of a specific type for all objects
void executeGlobalCallbacks(CallbackType type);

// Function to free all callback nodes
void freeCallbacks(Object_t* obj);

#endif
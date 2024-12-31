#ifndef LIBRARY_H
#define LIBRARY_H

#include "component.h"

typedef void (*EmitComponentInfoFn)(ComponentInfo_t*);
typedef void (*EmitInterfaceInfoFn)(InterfaceInfo_t*);
typedef void (*EmitTypeInfoFn)(BasicTypeInfo_t*);
typedef void (*RegisterCallbackFn)(ObjectId, CallbackType, uint32_t, CallbackFunc);
typedef void (*RegisterInterfaceFn)(ObjectId, InterfaceInfo_t*);


typedef struct {
    EmitComponentInfoFn EmitComponentInfo;
    EmitInterfaceInfoFn EmitInterfaceInfo;
    EmitTypeInfoFn      EmitTypeInfo;
    RegisterCallbackFn  RegisterCallback;
    RegisterInterfaceFn RegisterInterface;
} MainApi;

typedef struct {
    void (*EnumerateComponents)(uint32_t startId);
    void (*EnumerateTypes)(uint32_t startId);
    Component_t (*AllocComponent)(uint32_t internalId);
    void (*FreeComponent)(Component_t info);
    uint32_t (*RegisterType)(uint32_t typeID, ffi_type* typeInfo);
} LibraryApi;

LibraryApi EmitApi(void);

#endif // LIBRARY_H


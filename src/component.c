#include "component.h"

ffi_type* ffiInfo;
Object_t globalObjects[MAX_OBJECTS];

ObjectId allocateObject(uint32_t ident) {
    Object_t* obj = NULL;
    if (ident == 0)
    {
        for (int i = 0; i < MAX_OBJECTS; ++i) {
            if (globalObjects[i].id == 0) {
                ident = i+1;
                break;
            }
        } 
    }

    if (ident == 0)
    {
        printf("Error: Maximum number of global objects reached!\n");
        return 0;
    }
    
    obj = &globalObjects[ident-1];
    
    if (obj->id != 0)
    {
        return obj->id;
    }

    // Init
	obj->id = ident;
	obj->componentCount = 0;
	for (int j = 0; j < CALLBACK_COUNT; ++j) {
		obj->callbacks[j] = NULL;
	}
	obj->interfaceCount = 0;
	return ident;
}

void freeObject(ObjectId ident) {
    Object_t* obj = NULL;
    if (ident <= 0)
        return;
    obj = &globalObjects[ident-1];

    if (obj == NULL) 
        return;

    freeComponents(obj);
    freeCallbacks(obj);
    obj->id = 0;
    obj->componentCount = 0;
    obj->interfaceCount = 0;
    for (int i = 0; i < CALLBACK_COUNT; ++i) {
        obj->callbacks[i] = NULL;
    }
}

void addComponent(ObjectId ident, Component_t component) {
    Object_t* obj = NULL;
    if (ident <= 0)
        return;
    obj = &globalObjects[ident-1];

    if (obj->componentCount < MAX_COMPONENTS) {
    	uint32_t nextIndex = obj->componentCount++;
        component.ref = nextIndex+1;
        obj->components[nextIndex] = component;
        if (component.info->registerCallbacks) {
            component.info->registerCallbacks(obj->id, &component);
        }
        if (component.info->enumerateInterfaces) {
            component.info->enumerateInterfaces(obj->id, &component);
        }
    } else {
        printf("Error: Maximum number of components reached!\n");
    }
}

void registerCallback(ObjectId ident, CallbackType type, ComponentRefId componentRef, CallbackFunc callback) {
    if (type >= CALLBACK_COUNT) {
        printf("Error: Invalid callback type!\n");
        return;
    }

    Object_t* obj = NULL;
    if (ident <= 0)
        return;
    obj = &globalObjects[ident-1];
    
    CallbackNode_t* node = (CallbackNode_t*)malloc(sizeof(CallbackNode_t));
    if (node) {
        node->component = componentRef;
        node->func = callback;
        node->next = obj->callbacks[type];
        obj->callbacks[type] = node;
    } else {
        printf("Error: Failed to allocate memory for callback node!\n");
    }
}

void registerInterface(ObjectId ident, InterfaceInfo_t* info) {
    Object_t* obj = NULL;
    if (ident <= 0)
        return;
    obj = &globalObjects[ident-1];
    
    if (obj->interfaceCount < MAX_INTERFACES) {
        obj->interfaces[obj->interfaceCount] = info;
        obj->interfaceCount++;
    } else {
        printf("Error: Maximum number of interfaces reached!\n");
    }
}

InterfaceInfo_t* queryInterface(ObjectId ident, const char* name) {
    Object_t* obj = NULL;
    if (ident <= 0)
        return NULL;
    obj = &globalObjects[ident-1];
    
    for (int i = 0; i < obj->interfaceCount; ++i) {
        if (strcmp(obj->interfaces[i]->name, name) == 0) {
            return obj->interfaces[i];
        }
    }
    return NULL; // Not found
}

void executeGlobalCallbacks(CallbackType type) {
    if (type >= CALLBACK_COUNT) {
        printf("Error: Invalid callback type for execution!\n");
        return;
    }

    for (int i = 0; i < MAX_OBJECTS; ++i) {
        if (globalObjects[i].id != 0) {
            Object_t* obj = &globalObjects[i];
            CallbackNode_t* current = obj->callbacks[type];
            while (current) {
                if (current->func) {
                    current->func(obj->id, &obj->components[current->component]);
                }
                current = current->next;
            }
        }
    }
}

void freeComponents(Object_t* obj) {
}

void freeCallbacks(Object_t* obj) {
    for (int i = 0; i < CALLBACK_COUNT; ++i) {
        CallbackNode_t* current = obj->callbacks[i];
        while (current) {
            CallbackNode_t* next = current->next;
            free(current);
            current = next;
        }
        obj->callbacks[i] = NULL;
    }
}




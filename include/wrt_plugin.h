#ifndef WRT_PLUGIN_H
#define WRT_PLUGIN_H
#endif

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WRT_LOAD_API(name, method, T) if(strcmp(name, #T) == 0) T = method

typedef void WrenVM;
typedef void* WrenHandle;
typedef enum
{
  WREN_RESULT_SUCCESS,
  WREN_RESULT_COMPILE_ERROR,
  WREN_RESULT_RUNTIME_ERROR
} WrenInterpretResult;
typedef enum
{
  WREN_TYPE_BOOL,
  WREN_TYPE_NUM,
  WREN_TYPE_FOREIGN,
  WREN_TYPE_LIST,
  WREN_TYPE_MAP,
  WREN_TYPE_NULL,
  WREN_TYPE_STRING,

  // The object is of a type that isn't accessible by the C API.
  WREN_TYPE_UNKNOWN
} WrenType;

static const int (*wrenGetSlotCount)(WrenVM*);
static const void (*wrenEnsureSlots)(WrenVM*, int);
static const WrenType (*wrenGetSlotType)(WrenVM*, int);
static const bool (*wrenGetSlotBool)(WrenVM*, int);
static const char* (*wrenGetSlotBytes)(WrenVM* vm, int slot, int* length);
static const double (*wrenGetSlotDouble)(WrenVM*, int);
static const void* (*wrenGetSlotForeign)(WrenVM*, int);
static const char* (*wrenGetSlotString)(WrenVM*, int);
static WrenHandle* (*wrenGetSlotHandle)(WrenVM*, int);
static const void (*wrenSetSlotBool)(WrenVM*, int, bool);
static const void (*wrenSetSlotBytes)(WrenVM*, int, const char*, size_t);
static const void (*wrenSetSlotDouble)(WrenVM*, int, double);
static const void* (*wrenSetSlotNewForeign)(WrenVM* vm, int slot, int classSlot, size_t size);
static const void (*wrenSetSlotNewList)(WrenVM*, int);
static const void (*wrenSetSlotNewMap)(WrenVM*, int);
static const void (*wrenSetSlotNull)(WrenVM*, int);
static const void (*wrenSetSlotString)(WrenVM*, int, const char*);
static const void (*wrenSetSlotHandle)(WrenVM*, int, WrenHandle*);
static const int (*wrenGetListCount)(WrenVM* vm, int slot);
static const void (*wrenGetListElement)(WrenVM* vm, int listSlot, int index, int elementSlot);
static const void (*wrenInsertInList)(WrenVM*, int listSlot, int index, int elementSlot);
static const int (*wrenGetMapCount)(WrenVM*, int);
static const bool (*wrenGetMapContainsKey)(WrenVM*, int, int);
static const void (*wrenGetMapValue)(WrenVM*, int, int, int);
static const void (*wrenSetMapValue)(WrenVM* vm, int mapSlot, int keySlot, int valueSlot);
static const void (*wrenRemoveMapValue)(WrenVM*, int, int, int);
static const void (*wrenReleaseHandle)(WrenVM* vm, WrenHandle* handle);
static const void (*wrenAbortFiber)(WrenVM* vm, int slot);
static WrenHandle* (*wrenMakeCallHandle)(WrenVM* vm, const char* signature);
static WrenInterpretResult (*wrenCall)(WrenVM* vm, WrenHandle* method);
static const void (*wrenGetVariable)(WrenVM* vm, const char* module, const char* name, int slot);
static WrenInterpretResult (*wrenInterpret)(WrenVM* vm, const char* module, const char* source);

typedef void (*WrenForeignMethodFn)(WrenVM* vm);
typedef void (*WrenFinalizerFn)(void* data);

static const void (*wrt_bind_method)(const char*, WrenForeignMethodFn);
static const void (*wrt_bind_class)(const char*, WrenForeignMethodFn, WrenFinalizerFn);
static const void (*wrt_wren_update_callback)(WrenForeignMethodFn fn);
static void (*wrt_set_plugin_data)(WrenVM* vm, int handle, void* value);
static void* (*wrt_get_plugin_data)(WrenVM* vm, int handle);
static WrenVM* (*wrt_new_wren_vm)();

void wrt_plugin_api(const char* name, void* method){
  WRT_LOAD_API(name, method, wrenGetSlotCount);
  WRT_LOAD_API(name, method, wrenEnsureSlots);
  WRT_LOAD_API(name, method, wrenGetSlotType);
  WRT_LOAD_API(name, method, wrenGetSlotBool);
  WRT_LOAD_API(name, method, wrenGetSlotBytes);
  WRT_LOAD_API(name, method, wrenGetSlotDouble);
  WRT_LOAD_API(name, method, wrenGetSlotForeign);
  WRT_LOAD_API(name, method, wrenGetSlotString);
  WRT_LOAD_API(name, method, wrenGetSlotHandle);
  WRT_LOAD_API(name, method, wrenSetSlotBool);
  WRT_LOAD_API(name, method, wrenSetSlotBytes);
  WRT_LOAD_API(name, method, wrenSetSlotDouble);
  WRT_LOAD_API(name, method, wrenSetSlotNewForeign);
  WRT_LOAD_API(name, method, wrenSetSlotNewList);
  WRT_LOAD_API(name, method, wrenSetSlotNewMap);
  WRT_LOAD_API(name, method, wrenSetSlotNull);
  WRT_LOAD_API(name, method, wrenSetSlotString);
  WRT_LOAD_API(name, method, wrenSetSlotHandle);
  WRT_LOAD_API(name, method, wrenGetListCount);
  WRT_LOAD_API(name, method, wrenGetListElement);
  WRT_LOAD_API(name, method, wrenInsertInList);
  WRT_LOAD_API(name, method, wrenGetMapCount);
  WRT_LOAD_API(name, method, wrenGetMapContainsKey);
  WRT_LOAD_API(name, method, wrenGetMapValue);
  WRT_LOAD_API(name, method, wrenSetMapValue);
  WRT_LOAD_API(name, method, wrenRemoveMapValue);
  WRT_LOAD_API(name, method, wrenReleaseHandle);
  WRT_LOAD_API(name, method, wrenAbortFiber);
  WRT_LOAD_API(name, method, wrenMakeCallHandle);
  WRT_LOAD_API(name, method, wrenCall);
  WRT_LOAD_API(name, method, wrenGetVariable);
  WRT_LOAD_API(name, method, wrenInterpret);
  
  WRT_LOAD_API(name, method, wrt_bind_method);
  WRT_LOAD_API(name, method, wrt_bind_class);
  WRT_LOAD_API(name, method, wrt_wren_update_callback);
  WRT_LOAD_API(name, method, wrt_get_plugin_data);
  WRT_LOAD_API(name, method, wrt_set_plugin_data);
  WRT_LOAD_API(name, method, wrt_new_wren_vm);
}

static void wren_runtime_error(WrenVM* vm, const char * error){
  wrenSetSlotString(vm, 0, error); 
  wrenAbortFiber(vm, 0);
}

#define METHOD(NAME) static void NAME(WrenVM* vm)
#define CONSTRUCTOR(NAME) static void NAME(WrenVM* vm)
#define DESTRUCTOR(NAME) static void NAME(void* data)

#ifdef __cplusplus
}
#endif
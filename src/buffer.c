#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <wren.h>
#include <wren_runtime.h>

static void wren_runtime_error(WrenVM* vm, const char * error){
  wrenSetSlotString(vm, 0, error); 
  wrenAbortFiber(vm, 0);
}

int plugin_handle;

typedef struct {
  size_t size;
  char* data;
}  Buffer;

inline static void check_buffer(Buffer* b, WrenVM* vm){
  if(b->data == NULL){
    wren_runtime_error(vm, "Out of memory");
  }
}

static void buffer_allocate(WrenVM* vm){
  (Buffer*)wrenSetSlotNewForeign(vm, 0, 0, sizeof(Buffer));
}

static void buffer_create_1(WrenVM* vm){
  Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
  //printf("%i\n", wrenGetSlotType(vm, 1));

  buffer->size = wrenGetSlotDouble(vm, 1);
  if(buffer->size != 0){
    buffer->data = (char*)calloc(buffer->size, sizeof(char));
    check_buffer(buffer, vm);
  }
}

static void buffer_free(void* data){
  Buffer* buffer = (Buffer*)data;
  if(buffer->data != NULL) free(buffer->data);
  buffer->size = 0;
  buffer->data = NULL;
}

static void buffer_resize_1(WrenVM* vm){
  Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
  buffer->size = wrenGetSlotDouble(vm, 1);
  buffer->data = (char*)realloc(buffer->data, buffer->size);
  check_buffer(buffer, vm);  
}

static void buffer_size(WrenVM* vm){
  Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
  wrenSetSlotDouble(vm, 0, buffer->size);
}

#define BUFFER_READ(NAME,TYPE) static void buffer_read_##NAME##_1(WrenVM* vm){\
  Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);\
  size_t offset = wrenGetSlotDouble(vm, 1);\
  if(offset + sizeof(TYPE) > buffer->size) { wren_runtime_error(vm, "Offset out of bounds"); return;  }\
  TYPE value = *((TYPE*)&buffer->data[offset]);\
  wrenSetSlotDouble(vm, 0 , value);\
}
#define BUFFER_READ_BIND(NAME) wrt_bind_method("buffer.Buffer.read" #NAME "(_)", buffer_read_##NAME##_1)

BUFFER_READ(Uint8,unsigned char)
BUFFER_READ(Uint16,unsigned short)
BUFFER_READ(Uint32,unsigned int)
BUFFER_READ(Int8,char)
BUFFER_READ(Int16,short)
BUFFER_READ(Int32,int)
BUFFER_READ(Float,float)
BUFFER_READ(Double,double)

#define BUFFER_WRITE(NAME, TYPE) static void buffer_write_##NAME##_2(WrenVM* vm){\
  Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);\
  size_t offset = wrenGetSlotDouble(vm, 1);\
  TYPE value = wrenGetSlotDouble(vm, 2);\
  if(offset + sizeof(TYPE) > buffer->size) { wren_runtime_error(vm, "Offset out of bounds"); return; }\
  *((TYPE*)&buffer->data[offset]) = value;\
}
#define BUFFER_WRITE_BIND(NAME) wrt_bind_method("buffer.Buffer.write" #NAME "(_,_)", buffer_write_##NAME##_2)

BUFFER_WRITE(Uint8,unsigned char)
BUFFER_WRITE(Uint16,unsigned short)
BUFFER_WRITE(Uint32,unsigned int)
BUFFER_WRITE(Int8,char)
BUFFER_WRITE(Int16,short)
BUFFER_WRITE(Int32,int)
BUFFER_WRITE(Float,float)
BUFFER_WRITE(Double,double)
  
static void buffer_toBytes_1(WrenVM* vm){
  Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
  size_t offset = wrenGetSlotDouble(vm, 1);
  if(offset > buffer->size) { wren_runtime_error(vm, "Offset out of bounds"); return; }
  wrenSetSlotBytes(vm, 0, (const char*)&buffer->data[offset], buffer->size - offset);
}

static void buffer_dispose_0(WrenVM* vm){
  Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
  buffer_free((void*)buffer);
}

static void buffer_isDisposed(WrenVM* vm){
  Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
  wrenSetSlotBool(vm, 0, buffer->data == NULL);
}

static void buffer_write_string_2(WrenVM* vm){
  Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
  size_t offset = wrenGetSlotDouble(vm,1);
  const char* str = wrenGetSlotString(vm,2);
  // get length including null-terminator
  size_t len = strlen(str)+1;
  if(offset+len > buffer->size) { wren_runtime_error(vm, "Offset out of bounds"); return; }
  memcpy(&buffer->data[offset], str, len);
  wrenSetSlotDouble(vm, 0, offset+len);
}

static void buffer_write_bytes_2(WrenVM* vm){
  Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
  size_t offset = wrenGetSlotDouble(vm,1);
  int len;
  const char* bytes = wrenGetSlotBytes(vm, 2, &len);
  if(offset+len > buffer->size) { wren_runtime_error(vm, "Offset out of bounds"); return; }
  memcpy(&buffer->data[offset], bytes, len);
  wrenSetSlotDouble(vm, 0, offset+len);
}

static void buffer_read_string_1(WrenVM* vm){
  Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
  size_t offset = wrenGetSlotDouble(vm,1);
  if(offset > buffer->size) { wren_runtime_error(vm, "Offset out of bounds"); return; }
  wrenSetSlotString(vm, 0, (const char*)&buffer->data[offset]);
}

static void buffer_read_bytes_2(WrenVM* vm){
  Buffer* buffer = (Buffer*)wrenGetSlotForeign(vm, 0);
  size_t offset = wrenGetSlotDouble(vm,1);
  size_t length = wrenGetSlotDouble(vm,2);
  if(offset+length > buffer->size) { wren_runtime_error(vm, "Offset out of bounds"); return; }
  wrenSetSlotBytes(vm, 0, (const char*)&buffer->data[offset], length);
}

static void buffer_copy_to_4(WrenVM* vm){
  Buffer* from = (Buffer*)wrenGetSlotForeign(vm, 0);
  Buffer* to = (Buffer*)wrenGetSlotForeign(vm, 1);
  size_t offsetSrc = wrenGetSlotDouble(vm,2);
  size_t offsetDst = wrenGetSlotDouble(vm,3);
  size_t length = wrenGetSlotDouble(vm,4);
  if(offsetSrc+length > from->size || offsetDst+length > to->size) { wren_runtime_error(vm, "Offset out of bounds"); return; }
  memcpy(&to->data[offsetDst], &from->data[offsetSrc], length);
}

WrenForeignMethodFn wrt_plugin_init_buffer(int handle){
  plugin_handle = handle;
  wrt_bind_class("buffer.Buffer", buffer_allocate, buffer_free);
  wrt_bind_method("buffer.Buffer.create_(_)", buffer_create_1);
  wrt_bind_method("buffer.Buffer.resize(_)", buffer_resize_1);
  wrt_bind_method("buffer.Buffer.toBytes(_)", buffer_toBytes_1);
  wrt_bind_method("buffer.Buffer.size", buffer_size);
  wrt_bind_method("buffer.Buffer.isDisposed", buffer_isDisposed);
  wrt_bind_method("buffer.Buffer.dispose()", buffer_dispose_0);
  wrt_bind_method("buffer.Buffer.writeString(_,_)", buffer_write_string_2);
  wrt_bind_method("buffer.Buffer.writeBytes(_,_)", buffer_write_bytes_2);
  wrt_bind_method("buffer.Buffer.readString(_)", buffer_read_string_1);
  wrt_bind_method("buffer.Buffer.readBytes(_,_)", buffer_read_bytes_2);
  wrt_bind_method("buffer.Buffer.copyTo(_,_,_,_)", buffer_copy_to_4);

  BUFFER_READ_BIND(Uint8);
  BUFFER_READ_BIND(Uint16);
  BUFFER_READ_BIND(Uint32);
  BUFFER_READ_BIND(Int8);
  BUFFER_READ_BIND(Int16);
  BUFFER_READ_BIND(Int32);
  BUFFER_READ_BIND(Float);
  BUFFER_READ_BIND(Double);
  BUFFER_WRITE_BIND(Uint8);
  BUFFER_WRITE_BIND(Uint16);
  BUFFER_WRITE_BIND(Uint32);
  BUFFER_WRITE_BIND(Int8);
  BUFFER_WRITE_BIND(Int16);
  BUFFER_WRITE_BIND(Int32);
  BUFFER_WRITE_BIND(Float);
  BUFFER_WRITE_BIND(Double);

  return NULL;
}
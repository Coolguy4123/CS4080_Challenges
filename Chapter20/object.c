
#include <stdio.h>
#include <string.h>

#include "memory.h"
#include "object.h"

#include "table.h"

#include "value.h"
#include "vm.h"


#define ALLOCATE_OBJ(type, objectType) \
    (type*)allocateObject(sizeof(type), objectType)



static Obj* allocateObject(size_t size, ObjType type) {
  Obj* object = (Obj*)reallocate(NULL, 0, size);
  object->type = type;

  object->isMarked = false;



  object->next = vm.objects;
  vm.objects = object;



#ifdef DEBUG_LOG_GC
  printf("%p allocate %zu for %d\n", (void*)object, size, type);
#endif


  return object;
}


ObjBoundMethod* newBoundMethod(Value receiver,
                               ObjClosure* method) {
  ObjBoundMethod* bound = ALLOCATE_OBJ(ObjBoundMethod,
                                       OBJ_BOUND_METHOD);
  bound->receiver = receiver;
  bound->method = method;
  return bound;
}


ObjClass* newClass(ObjString* name) {
  ObjClass* klass = ALLOCATE_OBJ(ObjClass, OBJ_CLASS);
  klass->name = name;

  initTable(&klass->methods);

  return klass;
}


ObjClosure* newClosure(ObjFunction* function) {

  ObjUpvalue** upvalues = ALLOCATE(ObjUpvalue*,
                                   function->upvalueCount);
  for (int i = 0; i < function->upvalueCount; i++) {
    upvalues[i] = NULL;
  }


  ObjClosure* closure = ALLOCATE_OBJ(ObjClosure, OBJ_CLOSURE);
  closure->function = function;

  closure->upvalues = upvalues;
  closure->upvalueCount = function->upvalueCount;

  return closure;
}


ObjFunction* newFunction() {
  ObjFunction* function = ALLOCATE_OBJ(ObjFunction, OBJ_FUNCTION);
  function->arity = 0;

  function->upvalueCount = 0;

  function->name = NULL;
  initChunk(&function->chunk);
  return function;
}


ObjInstance* newInstance(ObjClass* klass) {
  ObjInstance* instance = ALLOCATE_OBJ(ObjInstance, OBJ_INSTANCE);
  instance->klass = klass;
  initTable(&instance->fields);
  return instance;
}


ObjNative* newNative(NativeFn function) {
  ObjNative* native = ALLOCATE_OBJ(ObjNative, OBJ_NATIVE);
  native->function = function;
  return native;
}






ObjString* makeString(bool ownsChars, char* chars, int length) {
  ObjString* string = ALLOCATE_OBJ(ObjString, OBJ_STRING);
  string->ownsChars = ownsChars;
  string->length = length;
  string->chars = chars;
  return string;
}


ObjString* copyString(const char* chars, int length) {
  char* heapChars = ALLOCATE(char, length + 1);
  memcpy(heapChars, chars, length);
  heapChars[length] = '\0';
  return makeString(true, heapChars, length);
}

ObjUpvalue* newUpvalue(Value* slot) {
  ObjUpvalue* upvalue = ALLOCATE_OBJ(ObjUpvalue, OBJ_UPVALUE);

  upvalue->closed = NIL_VAL;

  upvalue->location = slot;

  upvalue->next = NULL;

  return upvalue;
}


static void printFunction(ObjFunction* function) {

  if (function->name == NULL) {
    printf("<script>");
    return;
  }

  printf("<fn %s>", function->name->chars);
}


void printObject(Value value) {
  switch (OBJ_TYPE(value)) {

    case OBJ_BOUND_METHOD:
      printFunction(AS_BOUND_METHOD(value)->method->function);
      break;


    case OBJ_CLASS:
      printf("%s", AS_CLASS(value)->name->chars);
      break;


    case OBJ_CLOSURE:
      printFunction(AS_CLOSURE(value)->function);
      break;


    case OBJ_FUNCTION:
      printFunction(AS_FUNCTION(value));
      break;


    case OBJ_INSTANCE:
      printf("%s instance",
             AS_INSTANCE(value)->klass->name->chars);
      break;


    case OBJ_NATIVE:
      printf("<native fn>");
      break;

    case OBJ_STRING:

      printf("%.*s", AS_STRING(value)->length, AS_CSTRING(value));
      break;

    case OBJ_UPVALUE:
      printf("upvalue");
      break;

  }
}

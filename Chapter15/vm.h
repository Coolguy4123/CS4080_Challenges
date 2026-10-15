#ifndef clox_vm_h
#define clox_vm_h

#include "chunk.h"
#include "value.h"

typedef struct {
  Chunk* chunk;
  uint8_t* ip;
  Value* stack;
  int stackCount;
  int stackCapacity;
} VM;

typedef enum {
  INTERPRET_OK,
  INTERPRET_COMPILE_ERROR,
  INTERPRET_RUNTIME_ERROR
} InterpretResult;

extern VM vm;

void initVM(void);
void freeVM(void);
InterpretResult interpret(Chunk* chunk);
void push(Value value);
Value pop(void);

#endif

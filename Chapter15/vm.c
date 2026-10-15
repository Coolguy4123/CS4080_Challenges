#include <stdio.h>

#include "debug.h"
#include "memory.h"
#include "vm.h"

VM vm;

static void resetStack(void) {
  vm.stackCount = 0;
}

static void growStack(void) {
  int oldCapacity = vm.stackCapacity;
  vm.stackCapacity = GROW_CAPACITY(oldCapacity);
  vm.stack = GROW_ARRAY(Value, vm.stack, oldCapacity, vm.stackCapacity);
}

void initVM(void) {
  vm.stack = NULL;
  vm.stackCapacity = 0;
  resetStack();
}

void freeVM(void) {
  FREE_ARRAY(Value, vm.stack, vm.stackCapacity);
  vm.stack = NULL;
  vm.stackCount = 0;
  vm.stackCapacity = 0;
}

void push(Value value) {
  if (vm.stackCapacity < vm.stackCount + 1) {
    growStack();
  }

  vm.stack[vm.stackCount] = value;
  vm.stackCount++;
}

Value pop(void) {
  vm.stackCount--;
  return vm.stack[vm.stackCount];
}

static InterpretResult run(void) {
#define READ_BYTE() (*vm.ip++)
#define READ_CONSTANT() (vm.chunk->constants.values[READ_BYTE()])
#define READ_CONSTANT_LONG()                                                \
  (vm.chunk->constants.values[vm.ip[0] | (vm.ip[1] << 8) | (vm.ip[2] << 16)])

  for (;;) {
#ifdef DEBUG_TRACE_EXECUTION
    printf("          ");
    for (Value* slot = vm.stack; slot < vm.stack + vm.stackCount; slot++) {
      printf("[ ");
      printValue(*slot);
      printf(" ]");
    }
    printf("\n");
    disassembleInstruction(vm.chunk, (int)(vm.ip - vm.chunk->code));
#endif

    uint8_t instruction;
    switch (instruction = READ_BYTE()) {
      case OP_CONSTANT: {
        Value constant = READ_CONSTANT();
        push(constant);
        break;
      }
      case OP_CONSTANT_LONG: {
        Value constant = READ_CONSTANT_LONG();
        vm.ip += 3;
        push(constant);
        break;
      }
      case OP_RETURN:
        printValue(pop());
        printf("\n");
        return INTERPRET_OK;
    }
  }

#undef READ_BYTE
#undef READ_CONSTANT
#undef READ_CONSTANT_LONG
}

InterpretResult interpret(Chunk* chunk) {
  vm.chunk = chunk;
  vm.ip = vm.chunk->code;
  return run();
}

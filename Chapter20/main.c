

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include "common.h"

#include "chunk.h"


#include "debug.h"


#include "vm.h"



static void repl() {
  char line[1024];
  for (;;) {
    printf("> ");

    if (!fgets(line, sizeof(line), stdin)) {
      printf("\n");
      break;
    }

    interpret(line);
  }
}


static char* readFile(const char* path) {
  FILE* file = fopen(path, "rb");

  if (file == NULL) {
    fprintf(stderr, "Could not open file \"%s\".\n", path);
    exit(74);
  }


  fseek(file, 0L, SEEK_END);
  size_t fileSize = ftell(file);
  rewind(file);

  char* buffer = (char*)malloc(fileSize + 1);

  if (buffer == NULL) {
    fprintf(stderr, "Not enough memory to read \"%s\".\n", path);
    exit(74);
  }


  size_t bytesRead = fread(buffer, sizeof(char), fileSize, file);

  if (bytesRead < fileSize) {
    fprintf(stderr, "Could not read file \"%s\".\n", path);
    exit(74);
  }


  buffer[bytesRead] = '\0';

  fclose(file);
  return buffer;
}


static void runFile(const char* path) {
  char* source = readFile(path);
  InterpretResult result = interpret(source);
  free(source);

  if (result == INTERPRET_COMPILE_ERROR) exit(65);
  if (result == INTERPRET_RUNTIME_ERROR) exit(70);
}

static void testTableKeys(void) {
  Table table;
  Value value;

  printf("Chapter 20 table key test\n");
  printf("1. Creating an empty table\n");
  initTable(&table);

  printf("2. Inserting a number key: 42 -> 100\n");
  tableSet(&table, NUMBER_VAL(42), NUMBER_VAL(100));
  printf("3. Inserting a Boolean key: true -> 200\n");
  tableSet(&table, BOOL_VAL(true), NUMBER_VAL(200));
  printf("4. Inserting a nil key: nil -> 300\n");
  tableSet(&table, NIL_VAL, NUMBER_VAL(300));

  printf("5. Looking up the number key: ");
  if (tableGet(&table, NUMBER_VAL(42), &value)) {
    printValue(value);
    printf("\n");
  }

  printf("6. Looking up the Boolean key: ");
  if (tableGet(&table, BOOL_VAL(true), &value)) {
    printValue(value);
    printf("\n");
  }

  printf("7. Looking up the nil key: ");
  if (tableGet(&table, NIL_VAL, &value)) {
    printValue(value);
    printf("\n");
  }

  printf("8. Freeing the table\n");
  freeTable(&table);
  printf("Table key test completed.\n\n");
}


int main(int argc, const char* argv[]) {

  initVM();
  testTableKeys();

  if (argc == 1) {
    repl();
  } else if (argc == 2) {
    runFile(argv[1]);
  } else {
    fprintf(stderr, "Usage: clox [path]\n");
    exit(64);
  }

  freeVM();



  return 0;
}



#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


#include "common.h"

#include "chunk.h"


#include "debug.h"


#include "object.h"


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

static double elapsedSeconds(clock_t start, clock_t end) {
  return (double)(end - start) / CLOCKS_PER_SEC;
}


static void benchmarkNumericInserts(int count) {
  Table table;
  clock_t start;

  initTable(&table);
  start = clock();
  for (int i = 0; i < count; i++) {
    tableSet(&table, NUMBER_VAL(i), NUMBER_VAL(i));
  }
  clock_t end = clock();

  printf("Time for inserting %d number keys: %.6f seconds\n",
         count, elapsedSeconds(start, end));
  freeTable(&table);
}


static void benchmarkLookups(int count) {
  Table table;
  Value value;
  volatile int checksum = 0;
  clock_t start;

  initTable(&table);
  for (int i = 0; i < count; i++) {
    tableSet(&table, NUMBER_VAL(i), NUMBER_VAL(i));
  }

  start = clock();
  for (int i = 0; i < count; i++) {
    if (tableGet(&table, NUMBER_VAL(i), &value)) {
      checksum += (int)AS_NUMBER(value);
    }
  }
  clock_t end = clock();

  printf("Time for lookup: %.6f seconds\n", elapsedSeconds(start, end));
  freeTable(&table);
}


static void benchmarkCollisionLookups(int count) {
  Table table;
  Value* keys = malloc(sizeof(Value) * 32);
  Value value;
  volatile int checksum = 0;
  int found = 0;
  int candidate = count;
  clock_t start;

  if (keys == NULL) {
    fprintf(stderr, "Could not allocate collision keys.\n");
    exit(74);
  }

  initTable(&table);
  for (int i = 0; i < count; i++) {
    tableSet(&table, NUMBER_VAL(i), NUMBER_VAL(i));
  }

  uint32_t target = hashValue(NUMBER_VAL(candidate)) &
                    (table.capacity - 1);
  while (found < 32) {
    if ((hashValue(NUMBER_VAL(candidate)) & (table.capacity - 1)) == target) {
      keys[found++] = NUMBER_VAL(candidate);
      tableSet(&table, NUMBER_VAL(candidate), NUMBER_VAL(candidate));
    }
    candidate++;
  }

  start = clock();
  for (int i = 0; i < count; i++) {
    if (tableGet(&table, keys[i % 32], &value)) {
      checksum += (int)AS_NUMBER(value);
    }
  }
  clock_t end = clock();

  printf("Time for lookup with collisions: %.6f seconds\n",
         elapsedSeconds(start, end));
  free(keys);
  freeTable(&table);
}


static void benchmarkDeletes(int count) {
  Table table;
  clock_t start;

  initTable(&table);
  for (int i = 0; i < count; i++) {
    tableSet(&table, NUMBER_VAL(i), NUMBER_VAL(i));
  }

  start = clock();
  for (int i = 0; i < count; i++) {
    tableDelete(&table, NUMBER_VAL(i));
  }
  clock_t end = clock();

  printf("Time for deleting %d entries: %.6f seconds\n", count,
         elapsedSeconds(start, end));
  freeTable(&table);
}


static void runBenchmarks(void) {
  const int count = 50000;

  benchmarkNumericInserts(count);
  benchmarkLookups(count);
  benchmarkCollisionLookups(count);
  benchmarkDeletes(count);
}


int main(int argc, const char* argv[]) {

  initVM();
  runBenchmarks();

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


#include <stdio.h>

#include <string.h>



#include "object.h"

#include "memory.h"
#include "value.h"

void
initValueArray(ValueArray * array)
{
  array->values = NULL;
  array->capacity = 0;
  array->count = 0;
}

void
writeValueArray(ValueArray * array, Value value)
{
  if (array->capacity < array->count + 1) {
    int             oldCapacity = array->capacity;
    array->capacity = GROW_CAPACITY(oldCapacity);
    array->values = GROW_ARRAY(Value, array->values,
                               oldCapacity, array->capacity);
  }
  array->values[array->count] = value;
  array->count++;
}


void
freeValueArray(ValueArray * array)
{
  FREE_ARRAY(Value, array->values, array->capacity);
  initValueArray(array);
}


void
printValue(Value value)
{

#ifdef NAN_BOXING
  if (IS_BOOL(value)) {
    printf(AS_BOOL(value) ? "true" : "false");
  } else if (IS_NIL(value)) {
    printf("nil");
  } else if (IS_NUMBER(value)) {
    printf("%g", AS_NUMBER(value));
  } else if (IS_OBJ(value)) {
    printObject(value);
  }
#else




  switch (value.type) {
  case VAL_BOOL:
    printf(AS_BOOL(value) ? "true" : "false");
    break;
  case VAL_NIL:
    printf("nil");
    break;
  case VAL_NUMBER:
    printf("%g", AS_NUMBER(value));
    break;

  case VAL_OBJ:
    printObject(value);
    break;
  case VAL_EMPTY:
    printf("<empty>");
    break;

  }


#endif

}


bool
valuesEqual(Value a, Value b)
{

#ifdef NAN_BOXING

  if (IS_NUMBER(a) && IS_NUMBER(b)) {
    return AS_NUMBER(a) == AS_NUMBER(b);
  }
  if (IS_STRING(a) && IS_STRING(b)) {
    ObjString      *aString = AS_STRING(a);
    ObjString      *bString = AS_STRING(b);
    return aString->length == bString->length &&
      memcmp(aString->chars, bString->chars, aString->length) == 0;
  }
  return a == b;
#else

  if (a.type != b.type)
    return false;
  switch (a.type) {
  case VAL_BOOL:
    return AS_BOOL(a) == AS_BOOL(b);
  case VAL_NIL:
    return true;
  case VAL_NUMBER:
    return AS_NUMBER(a) == AS_NUMBER(b);


  case VAL_OBJ:
    if (IS_STRING(a) && IS_STRING(b)) {
      ObjString      *aString = AS_STRING(a);
      ObjString      *bString = AS_STRING(b);
      return aString->length == bString->length &&
        memcmp(aString->chars, bString->chars,
               aString->length) == 0;
    }
    return AS_OBJ(a) == AS_OBJ(b);
  case VAL_EMPTY:
    return true;

  default:
    return false;
  }

#endif

}

static uint32_t hashDouble(double value){
  union BitCast {
    double          value;
    uint32_t        ints[2];
  };

  union BitCast   cast;
  cast.value = value + 1.0;
  return cast.ints[0] + cast.ints[1];
}

uint32_t
hashValue(Value value)
{
  switch (value.type) {
  case VAL_BOOL:
    return AS_BOOL(value) ? 3 : 5;
  case VAL_NIL:
    return 7;
  case VAL_NUMBER:
    return hashDouble(AS_NUMBER(value));
  case VAL_OBJ:
    return AS_STRING(value)->hash;
  case VAL_EMPTY:
    return 0;
  }
}

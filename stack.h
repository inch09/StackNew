#ifndef stack_h
#define stack_h
#include "stack.h"
#include "config.h"
#include <TXLib.h>


struct Stack_t{

    ON_HASHES(ssize_t hashData;
              ssize_t hashFullStruct;)
    ON_DBG(const char* name;
           const char* file;
           int line;)
    Stack_elem_t* data;
    size_t size;
    size_t capacity;
    
};

enum Errors{
    NO_ERR,
    NULL_STACK_POINTER,
    NULL_STACK_DATA_POINTER,
    ZERO_CAPACITY_ERR,
    NEGATIVE_CAPACITY_ERR,
    SIZE_MORE_THAN_CAPACITY_ERR,
    NEGATIVE_SIZE_ERR
    ON_CANARIES(
        ,VALUE_OF_LEFT_CANARY_CHANGED_ERR,
        VALUE_OF_RIGHT_CANARY_CHANGED_ERR)
    ON_HASHES(
        ,HASH_DATA_DOES_NOT_MATCH_ERR,
        HASH_FULL_STRUCT_DOES_NOT_MATCH_ERR)
};

Errors stackInit(Stack_t* stk, size_t capacity 
                 ON_DBG(,const char* name, const char* file, int line));
Errors stackDestroy(Stack_t* stk);

Errors stackPush(Stack_t* stk, Stack_elem_t value);
Stack_elem_t stackPop(Stack_t* stk);

Errors stackDump(const char* fileName, Stack_t* stk);

Errors reallocUp(Stack_t* stk);
Errors reallocDown(Stack_t* stk);
void reallocArray(Stack_t* stk);

Errors stackError(Stack_t* stk);
const char* handleTheError(Errors err);

bool isEqual(double a, double b);

ON_HASHES(ssize_t calculateHashData(Stack_t* stk);
          ssize_t calculateHashFullStruct(Stack_t* stk);)

#endif

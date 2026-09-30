#include <TXLib.h>
#include <stdio.h>

#define STACK_VERIFICATION_MODE
#define STACK_DEBUG_MODE
/*--------------------------------------------------*/
#ifdef STACK_VERIFICATION_MODE
#define STACK_VERIFY(stackPtr){\
    if(stackError(stackPtr) != NO_ERR){\
        handleTheError(stackError(stackPtr));\
        assert(0);\
    }\
} 
#else
#define STACK_VERIFY(...)
#endif
/*---------------------------------------------------*/
#ifdef STACK_DEBUG_MODE
#define ON_DBG(...) __VA_ARGS__
#else
#define ON_DBG(...)
#endif
/*----------------------------------------------------*/
#define POISON 2396752


struct Stack_t{

    ON_DBG(const char* name;
           const char* file;
           int line);
    double* data;
    size_t size;
    size_t capacity;
    
};

enum Errors{//
    NO_ERR,
    NULL_STACK_POINTER,
    NULL_STACK_DATA_POINTER ,
    ZERO_CAPACITY_ERR,
    NEGATIVE_CAPACITY_ERR,
    SIZE_MORE_THAN_CAPACITY_ERR,
    NEGATIVE_SIZE_ERR
};

Errors stackInit(Stack_t* stk, size_t capacity 
                 ON_DBG(,const char* name, const char* file, int line));
Errors stackDestroy(Stack_t* stk);

Errors stackPush(Stack_t* stk, double value);
double stackPop(Stack_t* stk, Errors* err);

Errors stackDump(const char* fileName, Stack_t* stk);

Errors reallocUp(Stack_t* stk);
Errors reallocDown(Stack_t* stk);

Errors stackError(Stack_t* stk);
void handleTheError(Errors err);




Errors stackInit(Stack_t* stk, size_t capacity 
                 ON_DBG(,const char* name, const char* file, int line)){
    //chack errors
    //check capacity size
    assert(stk);
    stk->capacity = capacity;

    stk->data = (double*) calloc(stk->capacity, sizeof(stk->data[0]));
    assert(stk->data);

    stk->size = 0;

    ON_DBG(
        stk->name = name;
        stk->file = file;
        stk->line = line
    );

    for(size_t i = 0; i < capacity; i++){
        stk->data[i] = POISON;
    }

    STACK_VERIFY(stk);
    printf("Init complete\n");

    return NO_ERR;
}


Errors stackPush(Stack_t* stk, double value){
    STACK_VERIFY(stk);

    assert(stk->size != stk->capacity);
    if(stk->size + 1 == stk->capacity){
        reallocUp(stk);
    }
    stk->data[stk->size] = value;
    stk->size++;

    STACK_VERIFY(stk);

    return NO_ERR;
}


Errors stackDump(const char* fileName, Stack_t* stk){
    STACK_VERIFY(stk);

    FILE* filePtr = fopen(fileName, "w");
    assert(filePtr);


    fprintf(filePtr, "--------------------------------------------------------------------------------------------------------------------------\n");
    fprintf(filePtr, "                   Information about our stack: \n\n");

    ON_DBG(
    fprintf(filePtr, "Stack_t <%s> [0x%p] created by main() at <%s>: line %d\n\n", stk->name, (void*) stk, stk->file, stk->line)    
    );

    fprintf(filePtr, "    capacity = %lu\n", (unsigned long) stk->capacity);
    fprintf(filePtr, "    size = %lu\n\n", (unsigned long) stk->size);

    //dataPtr
    for(size_t i = 0; i < stk->capacity; i++){
        if(i == stk->size - 1){
            fprintf(filePtr, "     [%lu] = %lg *last element\n\n", (unsigned long) i, stk->data[i]);
            continue;
        }
        fprintf(filePtr, "     [%lu] = %lg\n", (unsigned long) i, stk->data[i]);
    }

    fprintf(filePtr, "----------------------------------------------------------------------------------------------------------------------\n");

    fclose(filePtr);

    STACK_VERIFY(stk);
    return NO_ERR;
}

Errors reallocUp(Stack_t* stk){
    STACK_VERIFY(stk);

    const size_t scaleFactor = 2;
    stk->capacity *= scaleFactor;

    stk->data = (double*) realloc((void*) stk->data, stk->capacity * sizeof(stk->data[0]));    
    assert(stk->data);

    for(size_t i = stk->size; i < stk->capacity; i++){
            stk->data[i] = POISON;
            //printf("data = %lg\n", stk->data[i]);
        }

    STACK_VERIFY(stk);

    return NO_ERR;
}


double stackPop(Stack_t* stk, Errors* err){
        
    STACK_VERIFY(stk);
    assert(err);
    //check errors to err
    //realloc
    assert(stk->size);
    double popValue = stk->data[stk->size - 1];
    stk->data[stk->size - 1] = POISON;
    stk->size--;

    reallocDown(stk);

    STACK_VERIFY(stk);

    return popValue;
}

Errors stackDestroy(Stack_t* stk){
    STACK_VERIFY(stk);

    // for(size_t i = 0; i < stk->size; i++){
    //     stk->data[i] =  CLEANING_CONSTANT;
    // }
    free(stk->data);

    stk->size = 0;
    stk->capacity = 0;

    stk->data = NULL;
    stk = NULL;

    return NO_ERR;
}

Errors stackError(Stack_t* stk){
    //size && capacity < 0
    if(stk == NULL){
        return NULL_STACK_POINTER;//todo
    }
    if(stk->data == NULL){
        return NULL_STACK_DATA_POINTER;
    }
    if(stk->capacity == 0){
        return ZERO_CAPACITY_ERR;
    }
    if(stk->capacity == (size_t) (-1)){
        return NEGATIVE_CAPACITY_ERR;
    }
    if(stk->size > stk->capacity){;
        return SIZE_MORE_THAN_CAPACITY_ERR;
    }
    if(stk->size == (size_t) (-1)){
        return NEGATIVE_SIZE_ERR;
    }

    return NO_ERR;
}

Errors reallocDown(Stack_t* stk){
    STACK_VERIFY(stk);

    const size_t scaleFactor = 2;
    const size_t compressCondition  = 4; 

    if(stk->size * compressCondition > stk->capacity){
        STACK_VERIFY(stk);
        return NO_ERR;
    }

    size_t oldCapacity = stk->capacity;
    stk->capacity /= scaleFactor;

    printf("size = %lu\n", (unsigned long) stk->size);
    printf("new capacity after reallocDown() = %lu\n", (unsigned long) stk->capacity);
    assert(stk->size < stk->capacity);

    // for(size_t i = stk->capacity; i < oldCapacity; i++){
    //     stk->data[i] = CLEANING_CONSTANT;
    // }

    stk->data = (double*) realloc((void*) stk->data, stk->capacity * sizeof(stk->data[0]));
    assert(stk->data);

    STACK_VERIFY(stk);

    return NO_ERR;
}

void handleTheError(Errors err){
    char* strError = "";

    switch (err){
    case NULL_STACK_POINTER:
        strError = "NULL_STACK_POINTER";
        break;
    case NULL_STACK_DATA_POINTER:
        strError = "NULL_STACK_DATA_POINTER";
        break;
    case ZERO_CAPACITY_ERR:
        strError = "ZERO_CAPACITY_ERR";
        break;
    case NEGATIVE_CAPACITY_ERR:
        strError = "NEGATIVE_CAPACITY_ERR";
        break;
    case SIZE_MORE_THAN_CAPACITY_ERR:
        strError = "SIZE_MORE_THAN_CAPACITY_ERR";
        break;
    case NEGATIVE_SIZE_ERR:
        strError = "NEGATIVE_SIZE_ERR";
        break; 
    case NO_ERR:
        return;
        break;        
    default:
        break;
    }

    printf("You have %s", strError);
}
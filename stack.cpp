#include <TXLib.h>
#include <stdio.h>


// typedef double Stack_elem_t;
// #define SPECIFIER "%lf"

// ---------------------------------------------------
#define STACK_VERIFICATION_MODE
#define STACK_DEBUG_MODE
//--------------------------------------------------
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
//---------------------------------------------------
#ifdef STACK_DEBUG_MODE
#define ON_DBG(...) __VA_ARGS__
#else
#define ON_DBG(...)
#endif
//----------------------------------------------------
#define POISON 2396752
#define LEFT_CANARY 676767
#define RIGHT_CANARY 525252
#define COUNT_OF_CANARY 2
#define SIZE_OF_CANARY_TYPE sizeof(double)


struct Stack_t{

    ON_DBG(const char* name;
           const char* file;
           int line);
    Stack_elem_t* data;
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
    NEGATIVE_SIZE_ERR,
    VALUE_OF_LEFT_CANARY_CHANGED_ERR,
    VALUE_OF_RIGHT_CANARY_CHANGED_ERR
};

Errors stackInit(Stack_t* stk, size_t capacity 
                 ON_DBG(,const char* name, const char* file, int line));
Errors stackDestroy(Stack_t* stk);

Errors stackPush(Stack_t* stk, Stack_elem_t value);
Stack_elem_t stackPop(Stack_t* stk, Errors* err);

Errors stackDump(const char* fileName, Stack_t* stk);

Errors reallocUp(Stack_t* stk);
Errors reallocDown(Stack_t* stk);

Errors stackError(Stack_t* stk);
void handleTheError(Errors err);

bool isEqual(double a, double b);
//bool isEqual(Stack_elem_t* ptrA, Stack_elem_t valueB, size_t sizeOfElem);


Errors stackInit(Stack_t* stk, size_t capacity 
                 ON_DBG(,const char* name, const char* file, int line)){
    //chack errors
    //check capacity size
    assert(stk);
    stk->capacity = capacity;
    
    double* canaryAddress = NULL;
    canaryAddress = (double*) malloc(stk->capacity * sizeof(stk->data[0]) + COUNT_OF_CANARY * SIZE_OF_CANARY_TYPE);
    assert(canaryAddress);

    stk->data = (Stack_elem_t*) ((char*) canaryAddress + SIZE_OF_CANARY_TYPE);
    assert(stk->data);

    *((double*) ((char*) stk->data - SIZE_OF_CANARY_TYPE)) = LEFT_CANARY;
    *((double*) ((char*) stk->data + stk->capacity * sizeof(stk->data[0]))) = RIGHT_CANARY;

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


Errors stackPush(Stack_t* stk, Stack_elem_t value){
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
    fprintf(filePtr, "    data address = [0x%p]\n", (void*) stk->data);
    //dataPtr
    for(size_t i = 0; i < stk->capacity; i++){
        if(i == stk->size - 1){
            fprintf(filePtr, "     [%lu] = " SPECIFIER " *last element\n\n", (unsigned long) i, stk->data[i]);
            continue;
        }
        fprintf(filePtr, "     [%lu] = " SPECIFIER "\n", (unsigned long) i, stk->data[i]);
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

    double* canaryAddress = NULL;
    canaryAddress = (double*) realloc((void*) ((char*) stk->data - SIZE_OF_CANARY_TYPE), stk->capacity * sizeof(stk->data[0]) + COUNT_OF_CANARY * SIZE_OF_CANARY_TYPE); 
    assert(canaryAddress);

    stk->data = (Stack_elem_t*) ((char*) canaryAddress + SIZE_OF_CANARY_TYPE);
    assert(stk->data);

    *((double*) ((char*) stk->data - SIZE_OF_CANARY_TYPE)) = LEFT_CANARY;
    *((double*) ((char*) stk->data + stk->capacity * sizeof(stk->data[0]))) = RIGHT_CANARY;

    for(size_t i = stk->size; i < stk->capacity; i++){
            stk->data[i] = POISON;
            //printf("data = %lg\n", stk->data[i]);
        }

    STACK_VERIFY(stk);

    return NO_ERR;
}


Stack_elem_t stackPop(Stack_t* stk, Errors* err){
        
    STACK_VERIFY(stk);
    assert(err);
    //check errors to err
    //realloc
    assert(stk->size);
    Stack_elem_t popValue = stk->data[stk->size - 1];
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
    if(!isEqual(*((double*) ((char*) stk->data - SIZE_OF_CANARY_TYPE)), LEFT_CANARY)){
        return VALUE_OF_LEFT_CANARY_CHANGED_ERR;
    }
    if(!isEqual(*((double*) ((char*) stk->data + stk->capacity * sizeof(stk->data[0]))), RIGHT_CANARY)){
        return VALUE_OF_RIGHT_CANARY_CHANGED_ERR;
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

    //size_t oldCapacity = stk->capacity;
    stk->capacity /= scaleFactor;

    printf("size = %lu\n", (unsigned long) stk->size);
    printf("new capacity after reallocDown() = %lu\n", (unsigned long) stk->capacity);
    assert(stk->size < stk->capacity);

    // for(size_t i = stk->capacity; i < oldCapacity; i++){
    //     stk->data[i] = CLEANING_CONSTANT;
    // }
    double* canaryAddress = NULL;
    canaryAddress = (double*) realloc((void*) ((char*) stk->data - SIZE_OF_CANARY_TYPE), stk->capacity * sizeof(stk->data[0]) + COUNT_OF_CANARY * SIZE_OF_CANARY_TYPE); 
    assert(canaryAddress);

    stk->data = (Stack_elem_t*) ((char*) canaryAddress + SIZE_OF_CANARY_TYPE);
    assert(stk->data);

    *((double*) ((char*) stk->data - SIZE_OF_CANARY_TYPE)) = LEFT_CANARY;
    *((double*) ((char*) stk->data + stk->capacity * sizeof(stk->data[0]))) = RIGHT_CANARY;

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
    case VALUE_OF_LEFT_CANARY_CHANGED_ERR:
        strError = "VALUE_OF_LEFT_CANARY_CHANGED_ERR";
        break;
    case VALUE_OF_RIGHT_CANARY_CHANGED_ERR:
        strError = "VALUE_OF_RIGHT_CANARY_CHANGED_ERR";
        break;
    case NO_ERR:
        return;
        break;        
    default:
        break;
    }

    printf("You have %s", strError);
}

bool isEqual(double a, double b){
    const double epsilon = 0.0001;
    return fabs(a - b) < epsilon;
}

// bool isEqual(Stack_elem_t* ptrA, Stack_elem_t valueB, size_t sizeOfElem){
//     bool isEqual = true;
//     assert(ptrA);

//     char* ptrValueB = NULL;
//     Stack_elem_t valB = valueB;
//     ptrValueB = (char*) valB;
//     assert(ptrValueB);
//     printf("dddd");

//     for(size_t i = 0; i < sizeOfElem; i++){
//         char a = *((char*) ptrA + i);
//         char b = *((char*) ptrValueB + i);
//         if(a != b){
//             isEqual = false;
//             break;
//         }     
//     }
//     return isEqual;
// }


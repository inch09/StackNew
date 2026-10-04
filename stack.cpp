#include <TXLib.h>
#include <stdio.h>

//todo __FUNCTION__
// hash more size_t
//hash po 8 byte idem
//rasshirit dump(oshibki )

// ---------------------------------------------------
#define STACK_VERIFICATION_MODE
#define STACK_DEBUG_MODE
#define STACK_WITH_CANARIES_MODE
#define STACK_WITH_HASHES_MODE
#define STACK_WITH_POISON_MODE
//--------------------------------------------------
#ifdef STACK_VERIFICATION_MODE
#define STACK_VERIFY(stackPtr){\
    if(stackError(stackPtr) != NO_ERR){\
        stackDump(LOG_FILE, stackPtr);\
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
#ifdef STACK_WITH_CANARIES_MODE
#define ON_CANARIES(...) __VA_ARGS__
#else
#define ON_CANARIES(...)
#endif
//----------------------------------------------------
#ifdef STACK_WITH_HASHES_MODE
#define ON_HASHES(...) __VA_ARGS__
#else
#define ON_HASHES(...)
#endif
//----------------------------------------------------
#ifdef STACK_WITH_POISON_MODE
#define ON_POISON(...) __VA_ARGS__
#else
#define ON_POISON(...)
#endif
//----------------------------------------------------
#define POISON 2396752
#define LEFT_CANARY 67676767
#define RIGHT_CANARY 52525252
#define COUNT_OF_CANARY 2
#define SIZE_OF_CANARY_TYPE sizeof(double)


struct Stack_t{

    ON_HASHES(ssize_t hashValue;)
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
    NEGATIVE_SIZE_ERR,
    VALUE_OF_LEFT_CANARY_CHANGED_ERR,
    VALUE_OF_RIGHT_CANARY_CHANGED_ERR,
    HASH_DOES_NOT_MATCH_ERR
};

Errors stackInit(Stack_t* stk, size_t capacity 
                 ON_DBG(,const char* name, const char* file, int line));
Errors stackDestroy(Stack_t* stk);

Errors stackPush(Stack_t* stk, Stack_elem_t value);
Stack_elem_t stackPop(Stack_t* stk, Errors* err);

Errors stackDump(const char* fileName, Stack_t* stk);

Errors reallocUp(Stack_t* stk);
Errors reallocDown(Stack_t* stk);
void reallocArray(Stack_t* stk);

Errors stackError(Stack_t* stk);
const char* handleTheError(Errors err);

bool isEqual(double a, double b);

ON_HASHES(ssize_t calculateHash(Stack_t* stk);)

//bool isEqual(Stack_elem_t* ptrA, Stack_elem_t valueB, size_t sizeOfElem);


Errors stackInit(Stack_t* stk, size_t capacity 
                 ON_DBG(,const char* name, const char* file, int line)){
    //chack errors
    //check capacity size
    assert(stk);
    stk->capacity = capacity;
    
    double* canaryAddress = NULL;
    canaryAddress = (double*) malloc(stk->capacity * sizeof(stk->data[0]) 
                    ON_CANARIES(+ COUNT_OF_CANARY * SIZE_OF_CANARY_TYPE));
    assert(canaryAddress);

    stk->data = (Stack_elem_t*) ((char*) canaryAddress 
                 ON_CANARIES(+ SIZE_OF_CANARY_TYPE));
    assert(stk->data);

    ON_CANARIES(*((double*) ((char*) stk->data - SIZE_OF_CANARY_TYPE)) = LEFT_CANARY;)
    ON_CANARIES(*((double*) ((char*) stk->data + stk->capacity * sizeof(stk->data[0]))) = RIGHT_CANARY;)

    stk->size = 0;

    ON_DBG(
        stk->name = name;
        stk->file = file;
        stk->line = line
    );

    ON_POISON(for(size_t i = 0; i < capacity; i++){
        stk->data[i] = POISON;
    };)

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

    ON_HASHES(stk->hashValue = calculateHash(stk);)

    STACK_VERIFY(stk);

    return NO_ERR;
}


Errors stackDump(const char* fileName, Stack_t* stk){
    //STACK_VERIFY(stk);

    FILE* filePtr = fopen(fileName, "w");
    assert(filePtr);

    Errors err = stackError(stk);
    const char* strError =  handleTheError(err);
    //if errors printf dont work
    fprintf(filePtr, "--------------------------------------------------------------------------------------------------------------------------\n");
    fprintf(filePtr, "                   Information about our stack: \n\n");

    ON_DBG(
    fprintf(filePtr, "Stack_t <%s> [0x%p] created at <%s>: line %d\n\n", stk->name, (void*) stk, stk->file, stk->line)    
    );

    fprintf(filePtr, "    capacity = %lu\n", (unsigned long) stk->capacity);
    fprintf(filePtr, "    size = %lu\n", (unsigned long) stk->size);
    fprintf(filePtr, "    data address = [0x%p]\n\n", (void*) stk->data);
    fprintf(filePtr, "    LEFT CANARY = %li\n", (long int) LEFT_CANARY);
    //dataPtr
    for(size_t i = 0; i < stk->capacity; i++){
        if(i == stk->size - 1){
            fprintf(filePtr, "     [%lu] = " SPECIFIER " *last element\n\n", (unsigned long) i, stk->data[i]);
            continue;
        }
        fprintf(filePtr, "     [%lu] = " SPECIFIER "\n", (unsigned long) i, stk->data[i]);
    }
    fprintf(filePtr, "    RIGHT CANARY = %li\n\n", (long int) RIGHT_CANARY);

    fprintf(filePtr, "     ");
    fprintf(filePtr, strError);
    fprintf(filePtr, "\n");

    fprintf(filePtr, "----------------------------------------------------------------------------------------------------------------------\n");

    fclose(filePtr);

    //STACK_VERIFY(stk);
    return NO_ERR;
}

Errors reallocUp(Stack_t* stk){
    STACK_VERIFY(stk);

    const size_t scaleFactor = 2;
    stk->capacity *= scaleFactor;

    reallocArray(stk);

    ON_CANARIES(*((double*) ((char*) stk->data - SIZE_OF_CANARY_TYPE)) = LEFT_CANARY;)
    ON_CANARIES(*((double*) ((char*) stk->data + stk->capacity * sizeof(stk->data[0]))) = RIGHT_CANARY;)

    ON_POISON(
    for(size_t i = stk->size; i < stk->capacity; i++){
        stk->data[i] = POISON;
            //printf("data = %lg\n", stk->data[i]);
    };)

    STACK_VERIFY(stk);

    return NO_ERR;
}


Stack_elem_t stackPop(Stack_t* stk, Errors* err){
        
    STACK_VERIFY(stk);
    assert(err);
    assert(stk->size);
    
    Stack_elem_t popValue = stk->data[stk->size - 1];
    
    ON_POISON(stk->data[stk->size - 1] = POISON;)

    stk->size--;
    
    ON_HASHES(stk->hashValue = calculateHash(stk);)

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

    ON_CANARIES(
    if(!isEqual(*((double*) ((char*) stk->data - SIZE_OF_CANARY_TYPE)), LEFT_CANARY)){
        return VALUE_OF_LEFT_CANARY_CHANGED_ERR;
    }
    if(!isEqual(*((double*) ((char*) stk->data + stk->capacity * sizeof(stk->data[0]))), RIGHT_CANARY)){
        return VALUE_OF_RIGHT_CANARY_CHANGED_ERR;
    };)

    ON_HASHES(
    if(calculateHash(stk) != stk->hashValue){
        return HASH_DOES_NOT_MATCH_ERR;
    };)

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

    stk->capacity /= scaleFactor;

    printf("size = %lu\n", (unsigned long) stk->size);
    printf("new capacity after reallocDown() = %lu\n", (unsigned long) stk->capacity);
    assert(stk->size < stk->capacity);

    reallocArray(stk);

    ON_CANARIES(*((double*) ((char*) stk->data - SIZE_OF_CANARY_TYPE)) = LEFT_CANARY;)
    ON_CANARIES(*((double*) ((char*) stk->data + stk->capacity * sizeof(stk->data[0]))) = RIGHT_CANARY;)

    STACK_VERIFY(stk);

    return NO_ERR;
}

#define GET_ERR_(err)     \
    case err:             \
        strError = #err;  \
        break;

const char* handleTheError(Errors err){
    const char* strError = "";

    switch (err){
        GET_ERR_(NULL_STACK_POINTER);
        GET_ERR_(NULL_STACK_DATA_POINTER);
        GET_ERR_(ZERO_CAPACITY_ERR);
        GET_ERR_(NEGATIVE_CAPACITY_ERR);
        GET_ERR_(SIZE_MORE_THAN_CAPACITY_ERR);
        GET_ERR_(NEGATIVE_SIZE_ERR);

        ON_CANARIES(GET_ERR_(VALUE_OF_LEFT_CANARY_CHANGED_ERR);
                    GET_ERR_(VALUE_OF_RIGHT_CANARY_CHANGED_ERR);)

        ON_HASHES(GET_ERR_(HASH_DOES_NOT_MATCH_ERR);)

        GET_ERR_(NO_ERR);
    default:
        break;
    }

    return strError;

    //printf("You have %s", strError);
}

#undef GET_ERR_

bool isEqual(double a, double b){
    const double epsilon = 0.0001;
    return fabs(a - b) < epsilon;
}


ON_HASHES(ssize_t calculateHash(Stack_t* stk){

    ssize_t hashVal = 0;
    for(size_t i = 0; i < stk->size * sizeof(Stack_elem_t); i++){
        if(i % 2 == 0){
            hashVal += *((char*) stk->data + i) * 14; 
        }
        else{
            hashVal += *((char*) stk->data + i);
        }
    }

    return hashVal;
})

void reallocArray(Stack_t* stk){
    double* canaryAddress = NULL;
    canaryAddress = (double*) realloc((void*) ((char*) stk->data 
                     ON_CANARIES(- SIZE_OF_CANARY_TYPE)), stk->capacity * sizeof(stk->data[0]) 
                     ON_CANARIES(+ COUNT_OF_CANARY * SIZE_OF_CANARY_TYPE)); 
    assert(canaryAddress);

    stk->data = (Stack_elem_t*) ((char*) canaryAddress
                 ON_CANARIES(+ SIZE_OF_CANARY_TYPE));
    assert(stk->data);
    return;
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


#ifndef config_h
#define config_h
#include "config.h"
#include "stack.h"

typedef int Stack_elem_t;
#define SPECIFIER "%d"
#define LOG_FILE "stack.log"

#define STACK_WITHOUT_DEBUG_MODE
//-------------------------------------------------
#ifdef STACK_FULL_DEBUG_MODE

#define STACK_VERIFICATION_MODE
#define STACK_DEBUG_MODE
#define STACK_WITH_CANARIES_MODE
#define STACK_WITH_HASHES_MODE
#define STACK_WITH_POISON_MODE

#else
    #ifdef STACK_WITHOUT_DEBUG_MODE

    #else
        //SELECTED MODES:
        #define STACK_VERIFICATION_MODE
        #define STACK_DEBUG_MODE
    #endif
#endif
//--------------------------------------------------
#ifdef STACK_VERIFICATION_MODE
#define STACK_VERIFY(stackPtr){\
    if(stackError(stackPtr) != NO_ERR){\
        stackDump(LOG_FILE, stackPtr);\
        assert("CHECK ERROR" == "IN LOG FILE");\
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

#endif

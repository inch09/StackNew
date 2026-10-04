typedef int Stack_elem_t;
#define SPECIFIER "%d"

#define LOG_FILE "stack.log"
//macros init
// stackPop delete err

//---------------------------
#include <TXLib.h>
#include <stdio.h>
#include "stack.cpp"
//--------------------------
int main(){

    Errors err = NO_ERR;
    Stack_t stk = {};
    stackInit(&stk, 5
              ON_DBG(,"stk", __FILE__, __LINE__));

    stackPush(&stk, 135);
    stackPush(&stk, 3);
    stackPush(&stk, 2);
    stackPush(&stk, 5);
    stackPush(&stk, 12);
    stackPush(&stk, 145);
    
    stk.data[-1] = 42;

    stackPop(&stk, &err);
    stackPop(&stk, &err);
    stackPop(&stk, &err);
    stackPop(&stk, &err);
    stackPop(&stk, &err);
    stackPop(&stk, &err);
    //stackPop(&stk, &err);

    stackPush(&stk, 5);
    stackPush(&stk, 123);
    stackPush(&stk, 2);
    stackPush(&stk, 5);
    stackPush(&stk, 12);
    stackPush(&stk, 12);
    stackPush(&stk, 12);
    stackPush(&stk, 30);


    stackDump(LOG_FILE, &stk);

    return 0;
}



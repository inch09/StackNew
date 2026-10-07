
//---------------------------
#include <TXLib.h>
#include <stdio.h>
#include "stack.h"
//--------------------------
int main(){
    Stack_t stk = {};
    stackInit(&stk, 10
              ON_DBG(,"stk", __FILE__, __LINE__));

    stackPush(&stk, 135);
    stackPush(&stk, 3);
    stackPush(&stk, 2);
    stackPush(&stk, 5);
    stackPush(&stk, 12);
    stackPush(&stk, 145);

    //stk.data[7] = 4;

    stackPop(&stk);
    stackPop(&stk);
    stackPop(&stk);
    stackPop(&stk);
    stackPop(&stk);
    stackPop(&stk);
    //stackPop(&stk);

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



#include "bms_main.h"
typedef void (*fun_t)(void);

typedef struct 
{
    fun_t entryHandler;
    fun_t exitHandler;
}stateTrans_t;

extern stateTrans_t stateTransFunArray[STATE_MAX];

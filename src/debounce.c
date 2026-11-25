#include "debounce.h"

debounce_t overvoltageDebounce = {0};
debounce_t vcuComTimeOutDebounce = {0};
debounce_t vcuFetRequest = {0};
void debounceInit(void)
{
    overvoltageDebounce.runningCount = 0;
    overvoltageDebounce.debouncedVal = 0;
    overvoltageDebounce.instantaneousVal = 0;

    vcuComTimeOutDebounce.runningCount = 0;
    vcuComTimeOutDebounce.debouncedVal = 0;
    vcuComTimeOutDebounce.instantaneousVal = 0;

    vcuFetRequest.runningCount = 0;
    vcuFetRequest.debouncedVal = 0;
    vcuFetRequest.instantaneousVal = 0;
}
void debounceFun( uint16_t count ,debounce_t *debounce )
{
    if(debounce->instantaneousVal != debounce->debouncedVal)
    {
        if( (debounce->runningCount) < (count))
        {
            debounce->runningCount++;
        }
        else
        {
            debounce->debouncedVal = debounce->instantaneousVal;
            debounce->runningCount = 0u;
        }
    }    
    else 
    {
        debounce->runningCount = 0u;
    }  

}
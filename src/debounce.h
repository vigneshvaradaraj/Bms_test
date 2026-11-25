#include <stdint.h>
typedef struct
{
    bool debouncedVal; 
    bool instantaneousVal;
    uint16_t runningCount;

}debounce_t;
extern debounce_t overvoltageDebounce;
extern debounce_t vcuComTimeOutDebounce;
extern debounce_t vcuFetRequest;
void debounceFun( uint16_t count , volatile debounce_t *debounce );
#include "stdint.h"
#define MOVING_AVG_WINDOW_SIZE (20)
extern uint16_t movingAvgAdcCurrentVal[MOVING_AVG_WINDOW_SIZE];
void adcInit(void);
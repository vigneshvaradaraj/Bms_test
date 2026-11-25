#include "bms_main.h"
#include "driver/spi.h"

void initAFE(void)
{
    spiInit();
}


void DMA_ch2_Handler(void)
{
    /*current data updated*/
    xTaskNotify(currentMeasurementParseTask,0u,eNoAction);
}
void DMA_ch1_Handler(void)/*100ms sampling rate*/
{
    /*voltge and NTC data updated*/
    xTaskNotify(afeMeasurementParseTask,0u,eNoAction);
}
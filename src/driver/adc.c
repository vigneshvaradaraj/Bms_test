#include "adc.h"

uint16_t movingAvgAdcCurrentVal[MOVING_AVG_WINDOW_SIZE];
void adcInit(void)
{
    /*Init adc channel for current Sensor*/
    /*config channel should be read by DMA with circular config with size 
    of moving average vindow size*/
    initDMA_adcChannel(movingAvgAdcCurrentVal,continiousConversion,circularWrite);
}

void DMA_adcChannelISR()
{
    /*triggers once it finishes the moving avg buffer*/
    xTaskNotify(currentMeasurementParseTask,0);
}

#include "timer.h"
#include "can.h"

void timerInit_100ms(void)
{
    /*need to set the timer6 for 0.5ms sec interrupt*/
	//step 1: enable clock for timer6
	RCC->APB2ENR |= (1 << 15);//enable clock for TIMER14
	
	//for apb1 timer clock just dived the 16Mz by 16
	//RCC->CFGR |= (7 << 10);
	
	//cnt clock is 1u sec it needs to be devided by 16 to make the timer clock as 1Khz 
    /*1m sec is the source of the TIM6*/
	TIM14->PSC = 16000 - 1;/*Assume 16 Mhz internal oscillator is used as source*/
	
	//store 5 to auto reload preload register to generate event for every 5m sec
	TIM14->ARR = 100 - 1;
	
//	TIM6->CR1 = 0;
//	TIM6->CR1 |= (1 << 7);
//	TIM6->CR1 |= (1 << 0);
//	TIM6->CR1 |= (1 << 2);
	
	NVIC_EnableIRQ(TIM7_DAC_IRQn);

	TIM14->CR1 |= (1 << 0);
	//enable interrupt 
	TIM14->DIER |= (1 <<0);
}

void timerCallBack_100ms(void)
{
    xTaskNotify(cellBalTask,0u,eNoAction); // dis cell bal
    triggerDMACh_1();/*for reading cell voltage*/
	vcuTimeOutCounter--;
	if(vcuTimeOutCounter < 0)
	{
		vcuTimeOutCounter = 0;
	}
}

void TIM7_DAC_IRQHandler(void)
{
    timerCallBack_100ms();
}
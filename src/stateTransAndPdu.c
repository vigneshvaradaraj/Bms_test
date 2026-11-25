
#include "stateTrans.h"
#include "
prechargeTimer.h"

typedef enum
{
	INIT_STATE = 0,
	STAND_BY_STATE,
	PRECHARGE_STATE,
	DRIVE_STATE,
	CHARGE_STATE,
	SLEEP_STATE,
	FAULT_STATE,
	STATE_MAX
}bmsState_t;

stateTrans_t stateTransFunArray[STATE_MAX];

void standByStateEntryHandler(void)
{
	/*suspend all the tasks except wakeup source*/
}

void standByStateExitHandler(void)
{
	/*resume all the tasks*/
}

void prechargeStateEntryHandler(void)
{
	if(0u == bmsFaultInfo.faultInfo && false == batteryPackData.EOC)
	{
		/*Engage precharge FET*/
		if (prechargeTimer != NULL)
		{
			xTimerStart(prechargeTimer, 0);
		}
	}
}

void prechargeStateExitHandler(void)
{
	/*Engage main FET*/

	/*disEngage precharge FET*/
}

void driveStateEntryHandler(void)
{
	/*suspend balancing task*/
}

void driveStateExitHandler(void)
{
	/*resume balancing task*/
}

void chargeStateEntryHandler(void)
{

}

void chargeStateExitHandler(void)
{

}

void sleepStateEntryHandler(void)
{

}

void sleepStateExitHandler(void)
{

}

void faultStateEntryHandler(void)
{
	/*DisEngage the FET*/
}

void faultStateExitHandler(void)
{
	
}


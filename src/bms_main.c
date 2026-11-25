
#include "bms_main.h"
#include "stateTransAndPdu.h"
#include "driver/can.h"
#include "driver/timer.h"
#include "debounce.h"
#include "balancing.h"
#include "driver/adc.h"

batteryPackData_t batteryPackData = {0};
afeData_t afeData = {0};

uint16_t rawCurrentData[NUM_CURRENT_SAMPLES];

bmsStateMachine_t bmsStateMachine = {0};
bmsFaultInfo_t bmsFaultInfo = { .faultInfo = 0U };
batteryPackFaultData_t batteryPackFaultData = {0};
cellBalInfo_t cellBalInfo[NUM_CELLS_IN_SERIES] = {0}; 

float currentConversionFactor = 0.1f;

TimerHandle_t prechargeTimer;

float parsingFactor = 0.1f;

TaskHandle_t debounceTaskHandle = NULL;
TaskHandle_t canComTaskHandle = NULL;
TaskHandle_t afeMeasurementParseTaskHandle = NULL;
TaskHandle_t currentMeasurementParseHandle = NULL;
TaskHandle_t stateTransTaskHandle = NULL;
TaskHandle_t cellBalTaskHandle = NULL;

void main(void)
{
	initSystemClock();
	initRtos();
	adcInit();
	timerInit_100ms();
	vTaskStartScheduler();
	
}

void initRtos(void)
{
	 BaseType_t status;

	prechargeTimer = xTimerCreate(
        "Precharge_Timer",
        pdMS_TO_TICKS((uint32_t)PRECHARGE_TIME),
        pdFALSE,
        NULL,
        prechargeTimeOut
    	);
	    status = xTaskCreate(debounceTask,"debounceTask",256,NULL,3,&debounceTaskHandle);
		if(pdPASS != status)
		{
			/*task not created*/
			return;
		}
		 status = xTaskCreate(canComTask,"canComTask",256,NULL,5,&canComTaskHandle);
		if(pdPASS != status)
		{
			/*task not created*/
			return;
		}
		 status = xTaskCreate(afeMeasurementParseTask,"afeMeasurementParseTask",256,NULL,3,&afeMeasurementParseTaskHandle);
		if(pdPASS != status)
		{
			/*task not created*/
			return;
		}
		 status = xTaskCreate(currentMeasurementParseTask,"currentMeasurementParseTask",256,NULL,3,&currentMeasurementParseHandle);
		if(pdPASS != status)
		{
			/*task not created*/
			return;
		}
		 status = xTaskCreate(stateTransTask,"stateTransTask",256,NULL,4,&stateTransTaskHandle);
		if(pdPASS != status)
		{
			/*task not created*/
			return;
		}
		 status = xTaskCreate(cellBalTask,"cellBalTask",256,NULL,4,&cellBalTaskHandle);
		if(pdPASS != status)
		{
			/*task not created*/
			return;
		}
}

void debounceTask(void* prm)
{
	while(1)
	{	
		debounceFun(OVER_VOLTAGE_DEBOUNCE_TIME,&overvoltageDebounce);
		vcuComTimeOutDebounce.instantaneousVal = getVcuTimeOut();
		debounceFun(VCU_COM_TIME_OUT_DEBOUNCE_TIME,&vcuComTimeOutDebounce);
		if(DRIVE_STATE == bmsStateMachine.currentState || CHARGE_STATE ==bmsStateMachine.currentState)
		{
			bmsStateMachine.nextState = SLEEP_STATE;
			xTaskNotify(stateTransTask,0);
		}
		debounceFun(VCU_FET_REQ_DEBOUNCE_TIME,&vcuFetRequest);
		if(DRIVE_STATE != bmsStateMachine.currentState || CHARGE_STATE != bmsStateMachine.currentState)
		{
			bmsStateMachine.nextState = DRIVE_STATE;
			xTaskNotify(stateTransTask,0);
		}
		xTaskDelay(pdMS_TO_TICKS(10));
	}
}

void canComTask(void* prm)
{
	uint32_t notifyVal = 0u;
	canData_t canData = {0};
	uint8_t voltageMuxCounter = 0u;
	while(1)
	{
		/*queue would be preferd for this instead of notification*/
		if(pdPASS == xTaskNotifyWait(0u,0u,&notifyVal,pdMS_TO_TICKS(MAX_MEASUREMENT_WAIT_TIME)))
		{
			switch(notifyVal)
			{
				case BMS_FAULTS_ID:
				{
					canData.Id = BMS_FAULTS_ID;
					canData.faremType = STANDARD;
					canData.dlc = 8;
					canData.dataArray[0] = (uint8_t)bmsFaultInfo.faultInfo;
					canData.dataArray[1] = (uint8_t)(bmsFaultInfo.faultInfo >> 8);
					canData.dataArray[6] = batteryPackFaultData.numOfActiveDTC;
					sendCanData(&canData);
					break;
				}
				case BMS_PACK_STATUS_ID:
				{
					canData.Id = BMS_PACK_STATUS_ID;
					canData.faremType = STANDARD;
					canData.dlc = 8;
					canData.data16_0 = batteryPackData.PackVoltage;
					canData.data16_1 = batteryPackData.packCurrent;
					canData.dataArray[4] = batteryPackData.soc;
					sendCanData(&canData);
					break;
				}
				case BMS_CELL_STATUS_ID:
				{
					canData.Id = BMS_CELL_STATUS_ID;
					canData.faremType = STANDARD;
					canData.dlc = 8;
					canData.data16_0 = batteryPackData.cellMax;
					canData.data16_1 = batteryPackData.cellMin;
					canData.dataArray[6] = batteryPackData.cellTempMax;
					canData.dataArray[7] = batteryPackData.cellTempMax;
					sendCanData(&canData);
					break;
				}
				case BMS_CELL_VOLTAGE_ID:
				{
					canData.Id = BMS_CELL_VOLTAGE_ID;
					canData.faremType = STANDARD;
					canData.dlc = 8;
					
					for(voltageMuxCounter = 0;voltageMuxCounter < NUM_CELLS_IN_SERIES;voltageMuxCounter+=3 )
					{
						canData.dataArray[0] = (uint8_t)voltageMuxCounter;
						canData.dataArray[1] = (uint8_t)(voltageMuxCounter >> 8);
						int i = 0;
						for(i=voltageMuxCounter;i<(voltageMuxCounter+3);i++)
						{
							canData.dataArray[((i-voltageMuxCounter)*2)+2] = (uint8_t)afeData.parsedCellData[i].cellVolatge;
							canData.dataArray[((i-voltageMuxCounter)*2)+3] = (uint8_t)(afeData.parsedCellData[i].cellVolatge >> 8);  
						}
						sendCanData(&canData);
						xTaskDelay(pdMS_TO_TICKS(100));
					}
				}
			}
		}
	}
}
void afeMeasurementParseTask(void* prm)
{
	uint16_t preFaultInfo = 0u; 
	while(1)
	{
		if(pdPASS == xTaskNotifyWait(0u,0u,NULL,pdMS_TO_TICKS(MAX_MEASUREMENT_WAIT_TIME)))
		{
			int cellParsingCount = 0;
			/*Parsing*/
			for(;cellParsingCount < NUM_CELLS_IN_SERIES;cellParsingCount++)
			{
				afeData.parsedCellData[cellParsingCount].cellVolatge = (uint16_t)((float)afeData.RawCellVoltages[cellParsingCount] * parsingFactor);
				cellBalInfo[cellParsingCount].voltage = afeData.parsedCellData[cellParsingCount].cellVolatge;
			}
			/*find min and max of cells */
			batteryPackData.cellMax = afeData.parsedCellData[0].cellVolatge;
			batteryPackData.cellMin = afeData.parsedCellData[0].cellVolatge;
			for(cellParsingCount = 0;cellParsingCount < NUM_CELLS_IN_SERIES;cellParsingCount++)
			{
				batteryPackData.cellMax = MAX(batteryPackData.cellMax,afeData.parsedCellData[cellParsingCount].cellVolatge);
				batteryPackData.cellMin = MIN(batteryPackData.cellMax,afeData.parsedCellData[cellParsingCount].cellVolatge);
			}
			/*update fault info*/
			if(batteryPackData.cellMax > CELL_VOLTAGE_MAX)
			{
				bmsFaultInfo.faultStruct.over_voltage = true;
			}
			if(batteryPackData.cellMin < CELL_VOLTAGE_MIN)
			{
				bmsFaultInfo.faultStruct.under_voltage = true;
			}
			/*notify CAN task if any change in fault*/
			if(preFaultInfo != bmsFaultInfo.faultInfo)
			{
				batteryPackFaultData.numOfActiveDTC = 0;
				for(cellParsingCount = 0;cellParsingCount < 16;cellParsingCount++)
				{
					if(bmsFaultInfo.faultInfo & (1 << cellParsingCount))
					{
						batteryPackFaultData.numOfActiveDTC++;
					}
				}
				xTaskNotify(canComTask,BMS_FAULTS_ID,eNoAction);
				preFaultInfo = bmsFaultInfo.faultInfo;
			}
			xTaskNotify(cellBalTask,1u,eNoAction); // enable cell bal
		}
	}
}

void currentMeasurementParseTask(void* prm)
{
	uint32_t packCurrentVal = 0;
	int index = 0;
	while(1)
	{
		if(pdPASS == xTaskNotifyWait(0u,0u,NULL,pdMS_TO_TICKS(MAX_MEASUREMENT_WAIT_TIME)))
		{
			for(;index <  MOVING_AVG_WINDOW_SIZE;index++)
			{
				packCurrentVal += movingAvgAdcCurrentVal[index];
			}
			packCurrentVal = packCurrentVal / MOVING_AVG_WINDOW_SIZE;
			packCurrentVal = packCurrentVal - ZERO_REF_CURRENT;
			batteryPackData.packCurrent =  (float)packCurrentVal * currentConversionFactor;
		}
	}
}

void stateTransTask(void* prm)
{
	/*This task will get notified when ever state changes*/
	while(1)
	{
		if(pdPASS == xTaskNotifyWait(0u,0u,NULL,pdMS_TO_TICKS(MAX_MEASUREMENT_WAIT_TIME)))
		{
			if(bmsStateMachine.currentState != bmsStateMachine.nextState)
			{
				stateTransFunArray[bmsStateMachine.currentState].exitHandler();
				bmsStateMachine.currentState = bmsStateMachine.nextState;
				stateTransFunArray[bmsStateMachine.currentState].entryHandler();
			}
		}
	}
}

void prechargeTimeOut(void)
{
	if(batteryPackData.packCurrent <= PRECHARGE_CURRENT_MIN)
	{
		bmsStateMachine.nextState = DRIVE_STATE;
		xTaskNotify(stateTransTask,0);
	}
}


void cellBalTask(void)
{
	int balCounter = 0;
	uint16_t cellMask = 0xAAAAu;
	uint16_t cellBalCount = 0u;
	uint16_t cellBalBits = 0u;
	uint32_t notifyVal = 0u;
	while(1)
	{
		if(pdPASS == xTaskNotifyWait(0u,0u,&notifyVal,pdMS_TO_TICKS(MAX_MEASUREMENT_WAIT_TIME)))
		{
			if(1 == notifyVal)
			{
				/*update balancing info*/
				for(balCounter = 0;balCounter < NUM_CELLS_IN_SERIES;balCounter++)
				{
					if((cellBalInfo[balCounter].voltage  - batteryPackData.cellMin > BAL_START_MV) && (afeData.parsedCellData[balCounter].cellVolatge > BAL_VMIN_MV))
					{
						cellBalInfo[balCounter].balneeded  = 1;
					}
					else if((cellBalInfo[balCounter].voltage - batteryPackData.cellMin < BAL_STOP_MV) || ((afeData.parsedCellData[balCounter].cellVolatge < BAL_VMIN_MV)))
					{
						cellBalInfo[balCounter].balneeded  = 0;
					}
				}
				preventBalancingAdjacentCells(cellBalInfo,&cellBalBits,BAL_MAX_CELLS);
				if(batteryPackData.cellTempMax > BAL_NTC_TEMP_MAX || batteryPackData.boardTemp > BAL_BOARD_TEMP_MAX || (bmsFaultInfo.faultInfo > 1))
				{
					afeCellBal(0);
				}
				else
				{
					afeCellBal(cellBalBits);
				}

			}
			else
			{
				afeCellBal(0);
			}
		}
	}
}
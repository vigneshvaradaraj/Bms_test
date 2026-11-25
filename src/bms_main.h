#include <stdint.h>
#define NUM_CELLS_IN_SERIES (16u)
#define NUM_CELLS_IN_PARALLEL (1u)
#define NUM_NTCS (3u)
#define NUM_CURRENT_SAMPLES (10u)
#define MAX_MEASUREMENT_WAIT_TIME (2000u)

#define PRECHARGE_CAP (0.1f)
#define PRECHARGE_RESISTER (10.0f)
#define PRECHARGE_TIME (5.0f * PRECHARGE_RESISTER * PRECHARGE_CAP)

#define ZERO_REF_CURRENT (2040) /*if it is 12bit adc */

#define CELL_VOLTAGE_MAX ()
#define CELL_VOLTAGE_MIN ()

#define OVER_VOLTAGE_DEBOUNCE_TIME (5) /*50ms*/
#define VCU_COM_TIME_OUT_DEBOUNCE_TIME (5) /*50ms*/
#define VCU_FET_REQ_DEBOUNCE_TIME (5) /*50ms*/

#define BAL_VMIN_MV (3400)



typedef struct afeData
{
    uint16_t RawCellVoltages[NUM_CELLS_IN_SERIES];
    uint16_t RawCellTemperature[NUM_NTCS];

	struct{
		uint16_t cellVolatge;
	}parsedCellData[NUM_CELLS_IN_SERIES];
	struct{
		uint16_t NTCtemp;
	}parsedNTCdata[NUM_NTCS];
}afeData_t;

typedef struct
{
	float PackVoltage;
	float packCurrent;
	float soc;
	bmsState_t currentBmsState;
	uint16_t cellMin;
	uint16_t cellMax;
	int16_t cellTempMax;
	int16_t cellTempMin;
	int16_t boardTemp;
	uint16_t balancingInfo;
	uint8_t EOD :1;
	uint8_t EOC :1;
}batteryPackData_t;

typedef struct 
{
	uint16_t faultBits;
	uint16_t warningBits;
	uint8_t numOfActiveDTC;
}batteryPackFaultData_t;

 
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

typedef enum
{
	INIT_EVENT
}initStateEvent_t;	

typedef enum
{
	DTC_REQUEST_EVENT,
	
}standByStateEvent_t;

typedef enum
{
	DTC_REQUEST_EVENT,
}prechargeStateEvent_t;

typedef enum
{
	CELL_BAL_EVENT,
	DTC_REQUEST_EVENT,
	
}driveStateEvent_t;

typedef enum
{
	CELL_BAL_EVENT,
	DTC_REQUEST_EVENT,
	
}chargeStateEvent_t;

typedef enum
{
	DTC_REQUEST_EVENT,
	
}faultStateEvent_t;
typedef enum
{
	
}sleepStateEvent_t;

typedef union
{

	uint16_t faultInfo;
	struct
	{
		uint8_t over_voltage : 1;
		uint8_t under_voltage : 1;
		uint8_t over_temperature : 1;
		uint8_t under_temperature : 1;
		uint8_t over_current : 1;
	}faultStruct;
	
}bmsFaultInfo_t;

typedef struct
{
	bmsState_t currentState;
	bmsState_t nextState;
}bmsStateMachine_t;

extern afeData_t afeData;
extern uint16_t rawCurrentData[NUM_CURRENT_SAMPLES];
extern batteryPackData_t batteryPackData;
extern bmsFaultInfo_t bmsFaultInfo;
extern TimerHandle_t prechargeTimer;
extern batteryPackFaultData_t batteryPackFaultData;
//	AFE_MEASUREMENT, shpuld be DMA based

void afeMeasurementParseTask(void* prm);
void currentMeasurementParseTask(void* prm);
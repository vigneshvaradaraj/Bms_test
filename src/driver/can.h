#include <stdint.h>
#include <stdbool.h>

#define BMS_PACK_STATUS_ID 0x350u
#define BMS_LIMITS_ID 0x351u
#define BMS_CELL_STATUS_ID 0x352u
#define BMS_CELL_VOLTAGE_ID 0x353u
#define BMS_FAULTS_ID 0x354u
#define VCU_COMMAND_ID 0x200u

#define ENGAGE_CONTACTOR_VAL (0x1)
#define DIS_ENGAGE_CONTACTOR_VAL (0x0)

#define VCU_COM_TIME_OUT (3) //300ms


typedef enum
{
    STANDARD,
    EXTENDED
}faremType_t;
typedef struct
{
    uint32_t Id;
    uint8_t dlc;
    faremType_t faremType;
    union
    {
        uint64_t data64;
        uint8_t dataArray[8];
        struct
        {
            uint32_t data32_1;
            uint32_t data32_0;
        };
        struct 
        {
            uint16_t data16_3;
            uint16_t data16_2;
            uint16_t data16_1;
            uint16_t data16_0;
        };
        struct
        {
            float data32_1f;
            float data32_0f;
        };
    };  
}canData_t;

extern int16_t vcuTimeOutCounter;
extern canData_t canDataRx;
void sendCanData(canData_t* canData);
bool getVcuTimeOut(void);
bool getVcuFetRequest(void);
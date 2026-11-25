#include "can.h"
int16_t vcuTimeOutCounter = 0;
bool engageContactor = 0;
canData_t canDataRx = {0};
void canInit()
{

}

void sendCanData(canData_t* canData)
{
    /*driver code*/
}
void readCanData(canData_t* canData)
{
    (void*)canData;
}
void canRxISR()
{
    readCanData(&canDataRx);
    switch(canDataRx.Id)
    {
        case VCU_COMMAND_ID:
        {
            if(ENGAGE_CONTACTOR_VAL == (canDataRx.dataArray[0] & 0x3))
            {
                engageContactor = true;
            }
            else if(DIS_ENGAGE_CONTACTOR_VAL == (canDataRx.dataArray[0] & 0x3))
            {
                engageContactor = false;
            }
            vcuTimeOutCounter = VCU_COM_TIME_OUT;
        }
    }
}

bool getVcuTimeOut(void)
{
    bool retVal = false;
    if(0 == vcuTimeOutCounter)
    {
        retVal = true;
    }
    return retVal;
}engageContactor

bool getVcuFetRequest(void)
{
    bool retVal = false;
    if(true == engageContactor)
    {
        retVal = true;
    }
    return retVal;
}
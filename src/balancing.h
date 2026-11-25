#include <stdint.h>
#include <stdbool.h>
#include "bms_main.h"

#define BAL_START_MV (15)
#define BAL_STOP_MV (5)
#define BAL_MAX_CELLS (8)
#define BAL_NTC_TEMP_MAX (50)
#define BAL_BOARD_TEMP_MAX (70)
#define BAL_DISCH_INHIBIT (2000)
#define BAL_MEAS_PAUSE (1)
#define BAL_TIMEOUT (8 * 60 * 60) //in sec

typedef struct
{
	uint16_t voltage;
	bool balneeded;
}cellBalInfo_t;

typedef struct 
{
    uint32_t value;
    uint32_t index;
} s_arrayWithIndex;

extern cellBalInfo_t cellBalInfo; 

void preventBalancingAdjacentCells(cellBalInfo_t cell[], uint16_t outputBalancingInfo,uint8_t maxBalCells)

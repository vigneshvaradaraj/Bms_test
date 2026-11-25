#include "balancing.h"
#include "bms_main.h"

static void arraySortDescending(int16_t arrayToSort[], int16_t arraySize, int16_t arraySorted[], int16_t indexSorted[])
{
    uint8_t i = 0; 
    uint8_t j = 0;

    /* declaration and initialization of "ArrayWithIndex" type structures */
    s_arrayWithIndex placeholderArray = {0};
    s_arrayWithIndex inputArrayWithIndex[NUM_CELLS_IN_SERIES] = {0};

    for (i = 0; i < arraySize; i++) 
    {
        inputArrayWithIndex[i].value = arrayToSort[i];
        inputArrayWithIndex[i].index = i;
    }

    /* values are sorted in descending order using the bubble sort method */
    for (i = 0; i < (arraySize - 1); i++)
    {
        for (j = 0; j < (arraySize - i - 1); j++)
        {
            if (inputArrayWithIndex[j + 1].value > inputArrayWithIndex[j].value) 
            {
                placeholderArray = inputArrayWithIndex[j];
                inputArrayWithIndex[j] = inputArrayWithIndex[j + 1];
                inputArrayWithIndex[j + 1] = placeholderArray;
            }
        }
    }

    /* assignment in the output variables of the array of values sorted in descending order and of the indices corresponding to the unsorted values of the original array */
    for (i = 0; i < arraySize; i++)
    {
        arraySorted[i] = inputArrayWithIndex[i].value;
        indexSorted[i] = inputArrayWithIndex[i].index;
    }
}

void preventBalancingAdjacentCells(cellBalInfo_t cell[], uint16_t outputBalancingInfo,uint8_t maxBalCells)
{
    int16_t cellVoltageArray[NUM_CELLS_IN_SERIES] = {0};
    int16_t cellVoltageArraySorted[NUM_CELLS_IN_SERIES] = {0};
    int16_t indexesAfterSorting[NUM_CELLS_IN_SERIES] = {0};
    bool needBalancingArray[NUM_CELLS_IN_SERIES] = {0};

    uint8_t i = 0;
    uint8_t j = 0;
    uint8_t k = 0;
    uint8_t cellIdx = 0;
    uint8_t numberOfCells = NUM_CELLS_IN_SERIES;

    uint8_t balCellCounts = 0u;

    /* classification of cell voltages in descending order */
    for (i = 0; i < numberOfCells; i++)
    {
        cellVoltageArray[i] = cell[i].voltage;
        needBalancingArray[i] = 0;
    }

    arraySortDescending(cellVoltageArray, numberOfCells, cellVoltageArraySorted, indexesAfterSorting);

    /* interpret the balancing orders and redo them to be sure that there are no adjacent cells to balance */
    if(numberOfCells != 0)
    {
        needBalancingArray[indexesAfterSorting[0]] = cell[indexesAfterSorting[0]].balneeded; //we take the balancing order of the cell which has the maximum voltage
        balCellCounts++;
    }

    //the following cells are tested in order of voltage, except for the min voltage cell
    for (i = 1; i < numberOfCells - 1; i++)
    {
        // we check if cells adjacent to cell # Indexes[i] are balancing
        /*what if the index is zero*/
        if (cell[indexesAfterSorting[i]].balneeded == 1 && needBalancingArray[indexesAfterSorting[i] + 1] != 1 && needBalancingArray[indexesAfterSorting[i] - 1] != 1)
        {
            needBalancingArray[indexesAfterSorting[i]] = 1;
            if(++balCellCounts >= balCellCounts)
            {
                break;
            }
        }
    }
    cellIdx = 0;
    /* Filling of the vector of the balance registers of the BQ from the physical vector and the masks */
    for (; cellIdx < NUM_CELLS_IN_SERIES; cellIdx++)
    {
        if(1 == needBalancingArray[cellIdx])
        {
            outputBalancingInfo |= 1 << cellIdx;
        }
        else
        {
            outputBalancingInfo &= ~(1 << cellIdx);
        }
    }
}
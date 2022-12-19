#include <Arduino.h>
#include "standard.h"
#include "mathf.h"

REAL32 mapf(REAL32 val, REAL32 in_min, REAL32 in_max, REAL32 out_min, REAL32 out_max)
{
    return(((val - in_min) * (out_max - out_min) / (in_max - in_min)) + out_min);
}
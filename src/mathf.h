#define LIMIT(value, min, max) ((value < (min)) ? (min) : ((value > (max)) ? (max) : value))
extern REAL32 mapf(REAL32 val, REAL32 in_min, REAL32 in_max, REAL32 out_min, REAL32 out_max);
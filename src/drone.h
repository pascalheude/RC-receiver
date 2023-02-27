#define MIN_ESC_COMMAND 192

extern BOOLEAN F_one_reception_ok;
extern BOOLEAN F_no_reception;
extern BOOLEAN r_toggle_switch;
extern BOOLEAN li_push_button;
extern BOOLEAN lo_push_button;
extern BOOLEAN ro_push_button;
extern INT16 l_potentiometer;
extern INT16 r_potentiometer;
extern REAL32 throttle;
extern REAL32 roll;
extern REAL32 pitch;
extern REAL32 yaw;

extern void initializeDrone(void);
extern void manageDrone(void);
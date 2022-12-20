#define MIN_ESC_COMMAND (REAL32)1000.0f
#define MAX_ESC_COMMAND (REAL32)2000.0f

extern UNS16 lf_esc_value;
extern UNS16 rf_esc_value;
extern UNS16 lr_esc_value;
extern UNS16 rr_esc_value;

extern void initializeEsc(void);
extern void driveEsc(void);
extern void stopEsc(void);
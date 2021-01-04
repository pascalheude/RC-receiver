typedef enum {
    LF = 0,
    RF,
    RR,
    LR
} T_esc_position;

extern UNS16 lf_esc_value;
extern UNS16 rf_esc_value;
extern UNS16 lr_esc_value;
extern UNS16 rr_esc_value;

extern void initializeEsc(void);
extern BOOLEAN calibrateEsc(T_esc_position esc_position);
extern void driveEsc(T_esc_position esc_position, UNS16 value);
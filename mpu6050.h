// Cordon SWIIC / Stemma QT QW-4CF
// Noir = Masse
// Rouge = VCC (+3.3V ou 5V)
// Bleu = SDA
// Jaune = SCL

#define YAW   0
#define PITCH 1
#define ROLL  2

extern REAL32 measure[];

extern void initializeMpu6050(void);
extern BOOLEAN calibrateMpu6050(void);
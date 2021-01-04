#include <Arduino.h>
#include <Wire.h>
#include "standard.h"
#include "mpu6050.h"

#define MPU6050_ADDRESS 0x68 // I2C address of the MPU-6050
#define SAMPLE_MAX 2048
#define X 0
#define Y 1
#define Z 2
#define RAD_TO_DEG 57.29577951308f
#define FREQ 250 // Sampling frequency
#define SSF_GYRO 65.5f // Sensitivity Scale Factor of the gyro from datasheet

REAL32 measure[3];

static BOOLEAN F_initialized;
static UNS16 calibration_loop;
static UNS16 temperature;
static REAL32 gyro_raw[3];
static REAL32 acc_raw[3];
static REAL32 gyro_offset[3];
static REAL32 gyro_angle[3];
static REAL32 acc_angle[3];

// Configure gyro and accelerometer precision
void setMpu6050Register(void)
{
    // Configure power management
    Wire.beginTransmission(MPU6050_ADDRESS); // Start communication with MPU
    Wire.write(0x6B);                    // Request the PWR_MGMT_1 register
    Wire.write(0x00);                    // Apply the desired configuration to the register
    Wire.endTransmission();              // End the transmission

    // Configure the gyro's sensitivity
    Wire.beginTransmission(MPU6050_ADDRESS); // Start communication with MPU
    Wire.write(0x1B);                    // Request the GYRO_CONFIG register
    Wire.write(0x08);                    // Apply the desired configuration to the register : ±500°/s
    Wire.endTransmission();              // End the transmission

    // Configure the acceleromter's sensitivity
    Wire.beginTransmission(MPU6050_ADDRESS); // Start communication with MPU
    Wire.write(0x1C);                    // Request the ACCEL_CONFIG register
    Wire.write(0x10);                    // Apply the desired configuration to the register : ±8g
    Wire.endTransmission();              // End the transmission

    // Configure low pass filter
    Wire.beginTransmission(MPU6050_ADDRESS); // Start communication with MPU
    Wire.write(0x1A);                    // Request the CONFIG register
    Wire.write(0x03);                    // Set Digital Low Pass Filter about ~43Hz
    Wire.endTransmission();              // End the transmission
}

void initializeMpu6050(void)
{
    F_initialized = false;
    calibration_loop = 0;
    temperature = 0;
    gyro_raw[X] = 0.0f;
    gyro_raw[Y] = 0.0f;
    gyro_raw[Z] = 0.0f;
    acc_raw[X] = 0.0f;
    acc_raw[Y] = 0.0f;
    acc_raw[Z] = 0.0f;
    gyro_offset[X] = 0.0f;
    gyro_offset[Y] = 0.0f;
    gyro_offset[Z] = 0.0f;
    acc_angle[X] = 0.0f;
    acc_angle[Y] = 0.0f;
    acc_angle[Z] = 0.0f;
    gyro_angle[X] = 0.0f;
    gyro_angle[Y] = 0.0f;
    gyro_angle[Z] = 0.0f;
    measure[ROLL] = 0.0f;
    measure[PITCH] = 0.0f;
    measure[YAW] = 0.0f;
    Wire.begin();
    Wire.setClock(400000);
    // TWBR = 12; // Set the I2C clock speed to 400kHz
    setMpu6050Register();
}

// Read raw values from MPU6050
void readSensor(void)
{
    UNS16 reading;

    Wire.beginTransmission(MPU6050_ADDRESS);// Start communicating with the MPU-6050
    Wire.write(0x3B);                   // Send the requested starting register
    Wire.endTransmission();             // End the transmission
    Wire.requestFrom(MPU6050_ADDRESS, 14);   // Request 14 bytes from the MPU-6050

    // Wait until all the bytes are received
    while(Wire.available() < 14)
    {
    }

    reading = Wire.read() << 8 | Wire.read(); // Add the low and high byte to the acc_raw[X] variable
    acc_raw[X] = (REAL32)reading;
    reading = Wire.read() << 8 | Wire.read(); // Add the low and high byte to the acc_raw[Y] variable
    acc_raw[Y] = (REAL32)reading;
    reading = Wire.read() << 8 | Wire.read(); // Add the low and high byte to the acc_raw[Z] variable
    acc_raw[Z] = (REAL32)reading;
    temperature = Wire.read() << 8 | Wire.read(); // Add the low and high byte to the temperature variable
    reading = Wire.read() << 8 | Wire.read(); // Add the low and high byte to the gyro_raw[X] variable
    gyro_raw[X] = (REAL32)reading;
    reading = Wire.read() << 8 | Wire.read(); // Add the low and high byte to the gyro_raw[Y] variable
    gyro_raw[Y] = (REAL32)reading;
    reading = Wire.read() << 8 | Wire.read(); // Add the low and high byte to the gyro_raw[Z] variable
    gyro_raw[Z] = (REAL32)reading;
}

// Calibrate MPU6050: take samples to calculate average offsets
// During this step, the quadcopter needs to be static and on a horizontal surface
BOOLEAN calibrateMpu6050(void)
{
    if (calibration_loop < SAMPLE_MAX)
    {
        readSensor();
        gyro_offset[X] += gyro_raw[X];
        gyro_offset[Y] += gyro_raw[Y];
        gyro_offset[Z] += gyro_raw[Z];
        calibration_loop++;
        return(false);
    }
    else
    {
        // Calculate average offsets
        gyro_offset[X] /= SAMPLE_MAX;
        gyro_offset[Y] /= SAMPLE_MAX;
        gyro_offset[Z] /= SAMPLE_MAX;
        return(true);
    }   
}

// Calculate pitch & roll angles using only the gyro
void calculateGyroAngle(void)
{
    // Subtract offsets
    gyro_raw[X] -= gyro_offset[X];
    gyro_raw[Y] -= gyro_offset[Y];
    gyro_raw[Z] -= gyro_offset[Z];

    // Angle calculation using integration
    gyro_angle[X] += (gyro_raw[X] / (FREQ * SSF_GYRO));
    gyro_angle[Y] += (-gyro_raw[Y] / (FREQ * SSF_GYRO)); // Change sign to match the accelerometer's one

    // Transfer roll to pitch if IMU has yawed
    gyro_angle[Y] += gyro_angle[X] * sin(gyro_raw[Z] / (REAL32)(FREQ * SSF_GYRO) / RAD_TO_DEG);
    gyro_angle[X] -= gyro_angle[Y] * sin(gyro_raw[Z] / (REAL32)(FREQ * SSF_GYRO) / RAD_TO_DEG);
}

// Calculate pitch & roll angles using only the accelerometer
void calculateAccelerometerAngle(void)
{
    REAL32 acc_total_vector;

    // Calculate total 3D acceleration vector : √(X² + Y² + Z²)
    acc_total_vector = sqrt((acc_raw[X] * acc_raw[X]) + (acc_raw[Y] * acc_raw[Y]) + (acc_raw[Z] * acc_raw[Z]));

    // To prevent asin to produce a NaN, make sure the input value is within [-1;+1]
    if (abs(acc_raw[X]) < acc_total_vector)
    {
        acc_angle[X] = RAD_TO_DEG * asin(acc_raw[X] / acc_total_vector); // asin gives angle in radian. Convert to degree multiplying by 180/pi
    }
    else if (acc_raw[X] > 0.0f)
    {
        acc_angle[X] = 90.0f;
    }
    else
    {
        acc_angle[X] = -90.0f;
    }

    if (abs(acc_raw[Y]) < acc_total_vector)
    {
        acc_angle[Y] = RAD_TO_DEG * asin(acc_raw[Y] / acc_total_vector);
    }
    else if (acc_raw[Y] > 0.0f)
    {
        acc_angle[Y] = 90.0f;
    }
    else
    {
        acc_angle[Y] = -90.0f;
    }
}

// Calculate real angles from gyro and accelerometer's values
void calculateAngle(void)
{
    calculateGyroAngle();
    calculateAccelerometerAngle();

    if (F_initialized)
    {
        // Correct the drift of the gyro with the accelerometer
        gyro_angle[X] = (gyro_angle[X] * 0.9996f) + (acc_angle[X] * 0.0004f);
        gyro_angle[Y] = (gyro_angle[Y] * 0.9996f) + (acc_angle[Y] * 0.0004f);
    }
    else
    {
        F_initialized = true;
        // At very first start, init gyro angles with accelerometer angles
        gyro_angle[X] = acc_angle[X];
        gyro_angle[Y] = acc_angle[Y];
    }
    // To dampen the pitch and roll angles a complementary filter is used
    measure[ROLL]  = (measure[ROLL]  * 0.9f) + (gyro_angle[X] * 0.1f);
    measure[PITCH] = (measure[PITCH] * 0.9f) + (gyro_angle[Y] * 0.1f);
    measure[YAW]   = -gyro_raw[Z] / SSF_GYRO; // Store the angular motion for this axis
}
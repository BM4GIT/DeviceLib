// file:   mpu9150.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.

#ifndef MPU9150_H
#define MPU9150_H

#include "sensor.h"

namespace MPU9150
{
    
class Compass : public Sensor
{
public:

    bool init();
    void setDeclination( float declination) { m_declination = declination; }
    void read();

    float pitch() { return m_pitch; }
    float roll() { return m_roll; }
    float yaw() { return m_yaw; }

    void calibrate( float magnetic_bias[3]);

protected:

    void initMPU9150();
    void initAK8975A(float * destination);
    void selfCalibrateMPU9150();
    void MPU6050SelfTest();
    float wrap(float x_h, float y_h);

    void readMagData( int16_t destination[3]);
    void readGyroData( int16_t destination[3]);
    void readAccelData( int16_t destination[3]);

    void readBytes( uint8_t address, uint8_t subAddress, uint8_t count, uint8_t* dest);
    uint8_t readByte( uint8_t address, uint8_t subAddress);
    void writeByte( uint8_t address, uint8_t subAddress, uint8_t data);


    int16_t accRaw[3];
    int16_t gyroRaw[3];
    int16_t compRaw[3];
    int16_t compRaw_Filter[3];

    float magCalibration[3] = {0,0,0};  // Factory mag calibration scale sensitivity values
    // User environmental x,y,z-axis correction in milliGauss (not in the raw values!)
    //float magBias[3] = {-5.0, 3.5, -18.0}; //Values for MPU9150 Sensor 1
    float magBias[3] = {1.8, 7.0, 8.0};     //Values for MPU9150 Sensor 2

    float accG[3];
    float gyroDegPerSec[3];
    float compMicroT[3];
    float ax, ay, az, gx, gy, gz, mx, my, mz, mx_h, my_h; // variables to hold sensor data above just for better overview here


    float pitchAcc, rollAcc, yawMag;  //Define variables for the "raw" angles calculated directly from the Acc values
    //Define variables for the angles incl. start values. Those angles are in degrees
    float m_pitch = 0;
    float m_roll = 0;
    float m_yaw = 0;  
    //roll and pitch are needed in radians for compass tilt compensation
    float pitchRad, rollRad;

    //Definition of variables for time and integral time calculation
    uint32_t now, lastupdate;
    double dT; //need to be precise due to short time intervalls

    float a_coeff = 0.95; // Coefficient for sensor fusion (how much weight put on the angle integration --> short term trust to gyro)
    float a_CompFilter = 0.9; // Coefficient for low pass filtering the raw comp data --> e.g. 0.9 is buidling the floating average of the last 10 values(?)

    float m_declination = 0;
};

} // end namespace

#endif

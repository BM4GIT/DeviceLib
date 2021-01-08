// file:   mpu9150.cpp
// Copyright 2020 D.E.Repolev
//
// This file is part of DeviceLib. DeviceLib is free software and you may distribute it under
// the terms of the GNU General Public License (version 3 or later) as published by the
// Free Software Foundation. The full license text you find at 'https://www.gnu.org/licenses'.
// Disclaimer: DeviceLib is distributed without any warranty.



// MPU-9150 Accelerometer + Gyro + Compass
// ---------------------------------------
// Code adapted and combined from:
// http://playground.arduino.cc/Main/MPU-9150
// https://github.com/kriswiner/MPU-9150
// http://www.instructables.com/id/Accelerometer-Gyro-Tutorial/step3/Combining-the-Accelerometer-and-Gyro/
// https://aerospace.honeywell.com/~/media/Images/Plymouth%20Website%20PDFs/Magnetic%20Sensors/Technical%20Articles/Applications_of_Magnetic_Sensors_for_Low_Cost_Compass_Systems.ashx
// 12/2020: Built a class based on an example of Tim Strohbach, 08/2015
//
// The general used coordinate system is given below. 
// X-Axis is pointing FORWARD in flight direction
// Y-Axis is pointing to the RIGHT side of the AC
// Z-Axis is pointing DOWNWARDS
// Rotation around x-axis is ROLL and clockwise positive OR a roll to the right is positive
// Rotation around y-axis is PITCH and clockwise positive OR nose up is positive
// Rotation around z-axis is YAW and clockwise positive OR a turn to east is positive
// 
// NOTE: The reference will be the coordinate system of the Magnetometer of the MPU9150 
//       Hence PRINTED X,Y on sensor are MISLEADING --> Change x and y in mind!!!!


#include "Wire.h"
#include "mpu9150.h"

// Register names according to the datasheet.
// According to the InvenSense document
// "MPU-9150 Register Map and Descriptions Revision 4.0",

#define MPU9150_XA_OFFSET_H        0x06 // User-defined trim values for accelerometer, populate with calibration routine
#define MPU9150_XA_OFFSET_L_TC     0x07
#define MPU9150_YA_OFFSET_H        0x08
#define MPU9150_YA_OFFSET_L_TC     0x09
#define MPU9150_ZA_OFFSET_H        0x0A
#define MPU9150_ZA_OFFSET_L_TC     0x0B
#define MPU9150_SELF_TEST_X        0x0D   // R/W
#define MPU9150_SELF_TEST_Y        0x0E   // R/W
#define MPU9150_SELF_TEST_Z        0x0F   // R/W
#define MPU9150_SELF_TEST_A        0x10   // R/W
#define MPU9150_XG_OFFS_USRH       0x13  // User-defined trim values for gyroscope, populate with calibration routine
#define MPU9150_XG_OFFS_USRL       0x14
#define MPU9150_YG_OFFS_USRH       0x15
#define MPU9150_YG_OFFS_USRL       0x16
#define MPU9150_ZG_OFFS_USRH       0x17
#define MPU9150_ZG_OFFS_USRL       0x18
#define MPU9150_SMPLRT_DIV         0x19   // R/W
#define MPU9150_CONFIG             0x1A   // R/W
#define MPU9150_GYRO_CONFIG        0x1B   // R/W
#define MPU9150_ACCEL_CONFIG       0x1C   // R/W
#define MPU9150_FF_THR             0x1D   // R/W
#define MPU9150_FF_DUR             0x1E   // R/W
#define MPU9150_MOT_THR            0x1F   // R/W
#define MPU9150_MOT_DUR            0x20   // R/W
#define MPU9150_ZRMOT_THR          0x21   // R/W
#define MPU9150_ZRMOT_DUR          0x22   // R/W
#define MPU9150_FIFO_EN            0x23   // R/W
#define MPU9150_I2C_MST_CTRL       0x24   // R/W
// Slave control registers from 0x25-0x35 not needed
#define MPU9150_I2C_MST_STATUS     0x36   // R
#define MPU9150_INT_PIN_CFG        0x37   // R/W
#define MPU9150_INT_ENABLE         0x38   // R/W
#define MPU9150_INT_STATUS         0x3A   // R 
// Data registers
#define ACCEL_XOUT_H       0x3B   // R  
#define ACCEL_XOUT_L       0x3C   // R  
#define ACCEL_YOUT_H       0x3D   // R  
#define ACCEL_YOUT_L       0x3E   // R  
#define ACCEL_ZOUT_H       0x3F   // R  
#define ACCEL_ZOUT_L       0x40   // R  
#define TEMP_OUT_H         0x41   // R  
#define TEMP_OUT_L         0x42   // R  
#define GYRO_XOUT_H        0x43   // R  
#define GYRO_XOUT_L        0x44   // R  
#define GYRO_YOUT_H        0x45   // R  
#define GYRO_YOUT_L        0x46   // R  
#define GYRO_ZOUT_H        0x47   // R  
#define GYRO_ZOUT_L        0x48   // R 

#define MPU9150_EXT_SENS_DATA_00   0x49   // R  
#define MPU9150_EXT_SENS_DATA_01   0x4A   // R  
#define MPU9150_EXT_SENS_DATA_02   0x4B   // R  
#define MPU9150_EXT_SENS_DATA_03   0x4C   // R  
#define MPU9150_EXT_SENS_DATA_04   0x4D   // R  
#define MPU9150_EXT_SENS_DATA_05   0x4E   // R  
#define MPU9150_EXT_SENS_DATA_06   0x4F   // R  
#define MPU9150_EXT_SENS_DATA_07   0x50   // R  
#define MPU9150_EXT_SENS_DATA_08   0x51   // R  
#define MPU9150_EXT_SENS_DATA_09   0x52   // R  
#define MPU9150_EXT_SENS_DATA_10   0x53   // R  
#define MPU9150_EXT_SENS_DATA_11   0x54   // R  
#define MPU9150_EXT_SENS_DATA_12   0x55   // R  
#define MPU9150_EXT_SENS_DATA_13   0x56   // R  
#define MPU9150_EXT_SENS_DATA_14   0x57   // R  
#define MPU9150_EXT_SENS_DATA_15   0x58   // R  
#define MPU9150_EXT_SENS_DATA_16   0x59   // R  
#define MPU9150_EXT_SENS_DATA_17   0x5A   // R  
#define MPU9150_EXT_SENS_DATA_18   0x5B   // R  
#define MPU9150_EXT_SENS_DATA_19   0x5C   // R  
#define MPU9150_EXT_SENS_DATA_20   0x5D   // R  
#define MPU9150_EXT_SENS_DATA_21   0x5E   // R  
#define MPU9150_EXT_SENS_DATA_22   0x5F   // R  
#define MPU9150_EXT_SENS_DATA_23   0x60   // R  
#define MPU9150_MOT_DETECT_STATUS  0x61   // R  
#define MPU9150_I2C_SLV0_DO        0x63   // R/W
#define MPU9150_I2C_SLV1_DO        0x64   // R/W
#define MPU9150_I2C_SLV2_DO        0x65   // R/W
#define MPU9150_I2C_SLV3_DO        0x66   // R/W
#define MPU9150_I2C_MST_DELAY_CTRL 0x67   // R/W
#define MPU9150_SIGNAL_PATH_RESET  0x68   // R/W
#define MPU9150_MOT_DETECT_CTRL    0x69   // R/W
#define MPU9150_USER_CTRL          0x6A   // R/W
#define MPU9150_PWR_MGMT_1         0x6B   // R/W
#define MPU9150_PWR_MGMT_2         0x6C   // R/W
#define MPU9150_FIFO_COUNTH        0x72   // R/W
#define MPU9150_FIFO_COUNTL        0x73   // R/W
#define MPU9150_FIFO_R_W           0x74   // R/W
#define MPU9150_WHO_AM_I           0x75   // R

//Adresses for DIRECT ACCESS of the AK8975A
#define AK8975A_ADDRESS  0x0C  // Adress of Compass in MPU9150 (see PS MPU9150 page 28)
#define WHO_AM_I_AK8975A 0x00 // should return 0x48
#define INFO             0x01
#define AK8975A_ST1      0x02  // data ready status bit 0
#define AK8975A_XOUT_L	 0x03  // data
#define AK8975A_XOUT_H	 0x04
#define AK8975A_YOUT_L	 0x05
#define AK8975A_YOUT_H	 0x06
#define AK8975A_ZOUT_L	 0x07
#define AK8975A_ZOUT_H	 0x08
#define AK8975A_ST2      0x09  // Data overflow bit 3 and data read error status bit 2
#define AK8975A_CNTL     0x0A  // Power down (0000), single-measurement (0001), self-test (1000) and Fuse ROM (1111) modes on bits 3:0
#define AK8975A_ASTC     0x0C  // Self test control
#define AK8975A_ASAX     0x10  // Fuse ROM x-axis sensitivity adjustment value
#define AK8975A_ASAY     0x11  // Fuse ROM y-axis sensitivity adjustment value
#define AK8975A_ASAZ     0x12  // Fuse ROM z-axis sensitivity adjustment value

// I2C addresses:
#define MPU9150_ADDRESS 0x68      //Adress of MPU6050 is 0x68 (AD0 set to Ground, see PS MPU9150 page 15)

// Definition of global constant variables
const float pi = 3.1415927;
const float RadToDeg = 57.29578;
// Declination Angle is 2 deg 11 min POSITIVE (to east) at 27412 Germany, can be found on  http://www.magnetic-declination.com/
const float declinationAngle = 2.183;  


/*** Specify sensor full scale and scale resolutions per LSB for the sensors ***/
// Possible accelerometer scales (and their register bit settings) are:
// 2 G (00), 4 G (01), 8 G (10), and 16 G  (11).
#define aRange 2.0       // 2G acceleration resolution (max. sensitivity)
#define Ascale 0         // Scale bit setting as 'int' --> e.g. 3 = 0b11 or 1 = 0b01
// Possible gyro scales (and their register bit settings) are
// 250 °/s (00), 500 °/s (01), 1000 °/s (10), and 2000 °/s  (11). 
#define gRange 250.0     // 250°/s gyro resolution (max. sensitivity)
#define Gscale 0         // Scale bit setting as 'int' --> e.g. 3 = 0b11 or 1 = 0b01

const float accRes = aRange/32768.0;  // 16 bit, 2G --> 0,000061035 g = per LSB
const float gyroRes = gRange/32768.0; // 16 bit, 250 deg/sec --> = per LSB
const float compRes = 1229.0/4096.0;  // = 0,3µT per LSB --> 1 Milligauss [mG] =   0,1 Mikrotesla [µT]

namespace MPU9150
{

void Compass::readBytes( uint8_t address, uint8_t subAddress, uint8_t count, uint8_t* dest)
{  
    Wire.beginTransmission( address);   // Initialize the Tx buffer
    Wire.write( subAddress);            // Put slave register address in Tx buffer
    Wire.endTransmission( false);       // Send the Tx buffer, but send a restart to keep connection alive
    uint8_t i = 0;
    Wire.requestFrom( address, count);  // Read bytes from slave register address 
    while ( Wire.available() ) {
        dest[i++] = Wire.read(); 
    }         // Put read results in the Rx buffer
}


uint8_t Compass::readByte( uint8_t address, uint8_t subAddress)
{
    uint8_t data; // `data` will store the register data	 
    Wire.beginTransmission( address);        // Initialize the Tx buffer
    Wire.write( subAddress);	               // Put slave register address in Tx buffer
    Wire.endTransmission( false);            // Send the Tx buffer, but send a restart to keep connection alive
    Wire.requestFrom(address, (uint8_t) 1);  // Read one byte from slave register address 
    data = Wire.read();                      // Fill Rx buffer with result
    return data;                             // Return data read from slave register
}

void Compass::writeByte( uint8_t address, uint8_t subAddress, uint8_t data)
{
    Wire.beginTransmission( address); // Initialize the Tx buffer
    Wire.write( subAddress);          // Put slave register address in Tx buffer
    Wire.write( data);                // Put data in Tx buffer
    Wire.endTransmission();           // Send the Tx buffer
}

void Compass::readAccelData( int16_t destination[3])
{
  uint8_t rawData[6];  // x/y/z accel register data stored here
  readBytes( MPU9150_ADDRESS, ACCEL_XOUT_H, 6, &rawData[0]);  // Read the six raw data registers into data array
  destination[0] = ((int16_t) rawData[0] << 8) | rawData[1] ;  // Turn the MSB and LSB into a signed 16-bit value
  destination[1] = ((int16_t) rawData[2] << 8) | rawData[3] ;  
  destination[2] = ((int16_t) rawData[4] << 8) | rawData[5] ; 
}


void Compass::readGyroData( int16_t destination[3])
{
  uint8_t rawData[6];  // x/y/z gyro register data stored here
  readBytes( MPU9150_ADDRESS, GYRO_XOUT_H, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
  destination[0] = ((int16_t) rawData[0] << 8) | rawData[1] ;  // Turn the MSB and LSB into a signed 16-bit value
  destination[1] = ((int16_t) rawData[2] << 8) | rawData[3] ;  
  destination[2] = ((int16_t) rawData[4] << 8) | rawData[5] ; 
}

void Compass::readMagData( int16_t destination[3])
{
    uint8_t rawData[6];  // x/y/z gyro register data stored here
    writeByte( AK8975A_ADDRESS, AK8975A_CNTL, 0x01); // toggle enable data read from magnetometer, no continuous read mode!
    delay( 10);
    // Only accept a new magnetometer data read if the data ready bit is set and 
    // if there are no sensor overflow or data read errors
    if( readByte( AK8975A_ADDRESS, AK8975A_ST1) & 0x01) { // wait for magnetometer data ready bit to be set
        readBytes( AK8975A_ADDRESS, AK8975A_XOUT_L, 6, &rawData[0]);  // Read the six raw data registers sequentially into data array
        destination[0] = ((int16_t) rawData[1] << 8) | rawData[0] ;  // Turn the MSB and LSB into a signed 16-bit value
        destination[1] = ((int16_t) rawData[3] << 8) | rawData[2] ;  
        destination[2] = ((int16_t) rawData[5] << 8) | rawData[4] ; 
    }
}


// Function for:
//   - Initializing the MPU9150/MPU6050 gyro and accelerometer
//   - Initializing AK8975A Magnetometer
//   - Calibrating the MPU9150/MPU6050 gyro and accelerometer
//   - Self test of the MPU9150/MPU6050 gyro and accelerometer to check health
//   - Calculating correct angles from x,y-values of the magnetormeter in the horizontal plane
//
// Most methods here are just sligthly adapted from: https://github.com/kriswiner/MPU-9150
// Special thanks to Kris Winer
//

void Compass::initMPU9150()
{  
 // wake up device
  writeByte(MPU9150_ADDRESS, MPU9150_PWR_MGMT_1, 0x00); // Clear sleep mode bit (6), enable all sensors 
  delay(100); // Delay 100 ms for PLL to get established on x-axis gyro; should check for PLL ready interrupt  

 // get stable time source
  writeByte(MPU9150_ADDRESS, MPU9150_PWR_MGMT_1, 0x01);  // Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
  delay(200);
  
 // Configure Gyro and Accelerometer
 // Disable FSYNC and set accelerometer and gyro bandwidth to 44 and 42 Hz, respectively; 
 // DLPF_CFG = bits 2:0 = 011; this sets the sample rate at 1 kHz for both
 // Minimum delay time is 4.9 ms which sets the fastest rate at ~200 Hz
 /*
 *          |   ACCELEROMETER    |           GYROSCOPE
 * DLPF_CFG | Bandwidth | Delay  | Bandwidth | Delay  | Sample Rate
 * ---------+-----------+--------+-----------+--------+-------------
 * 0        | 260Hz     | 0ms    | 256Hz     | 0.98ms | 8kHz
 * 1        | 184Hz     | 2.0ms  | 188Hz     | 1.9ms  | 1kHz
 * 2        | 94Hz      | 3.0ms  | 98Hz      | 2.8ms  | 1kHz
 * 3        | 44Hz      | 4.9ms  | 42Hz      | 4.8ms  | 1kHz
 * 4        | 21Hz      | 8.5ms  | 20Hz      | 8.3ms  | 1kHz
 * 5        | 10Hz      | 13.8ms | 10Hz      | 13.4ms | 1kHz
 * 6        | 5Hz       | 19.0ms | 5Hz       | 18.6ms | 1kHz
 * 7 RESERVED
 */
  writeByte(MPU9150_ADDRESS, MPU9150_CONFIG, 0x03);  
 
 // Set sample rate = gyroscope output rate/(1 + SMPLRT_DIV)
  writeByte(MPU9150_ADDRESS, MPU9150_SMPLRT_DIV, 0x04);  // Use a 200 Hz rate; the same rate set in CONFIG above
 
 // Set gyroscope full scale range
 // Range selects FS_SEL and AFS_SEL are 0 - 3, so 2-bit values are left-shifted into positions 4:3
  uint8_t c =  readByte(MPU9150_ADDRESS, MPU9150_GYRO_CONFIG);
  writeByte(MPU9150_ADDRESS, MPU9150_GYRO_CONFIG, c & ~0xE0); // Clear self-test bits [7:5] (0xE0 = 0b11100000)
  writeByte(MPU9150_ADDRESS, MPU9150_GYRO_CONFIG, c & ~0x18); // Clear AFS bits [4:3]       (0x18 = 0b00011000)
  writeByte(MPU9150_ADDRESS, MPU9150_GYRO_CONFIG, c | Gscale << 3); // Set full scale range for the gyro
   
 // Set accelerometer configuration
  c =  readByte(MPU9150_ADDRESS, MPU9150_ACCEL_CONFIG);
  writeByte(MPU9150_ADDRESS, MPU9150_ACCEL_CONFIG, c & ~0xE0); // Clear self-test bits [7:5] 
  writeByte(MPU9150_ADDRESS, MPU9150_ACCEL_CONFIG, c & ~0x18); // Clear AFS bits [4:3]
  writeByte(MPU9150_ADDRESS, MPU9150_ACCEL_CONFIG, c | Ascale << 3); // Set full scale range for the accelerometer 

  // Configure Interrupts and enable I2C_BYPASS_EN so additional chips 
  // can join the I2C bus and all can be controlled by the Arduino as master
  writeByte(MPU9150_ADDRESS, MPU9150_INT_PIN_CFG, 0x02);  // Enable bypass 
  writeByte(MPU9150_ADDRESS, MPU9150_USER_CTRL, 0x00);  //Disable the MPU6050 as Master --> I2C Bus control by Arduino
}

void Compass::initAK8975A(float * destination)
{
  uint8_t rawData[3];  // x/y/z gyro register data stored here
  writeByte(AK8975A_ADDRESS, AK8975A_CNTL, 0x00); // Power down
  delay(10);
  writeByte(AK8975A_ADDRESS, AK8975A_CNTL, 0x0F); // Enter Fuse ROM access mode
  delay(10);
  readBytes(AK8975A_ADDRESS, AK8975A_ASAX, 3, &rawData[0]);  // Read the x-, y-, and z-axis calibration values
  destination[0] =  (float)(rawData[0] - 128)/256. + 1.; // Return x-axis sensitivity adjustment values
  destination[1] =  (float)(rawData[1] - 128)/256. + 1.;  
  destination[2] =  (float)(rawData[2] - 128)/256. + 1.; 
  writeByte(AK8975A_ADDRESS, AK8975A_CNTL, 0x00); // Power down
  delay(10);
  Serial.println(F("Magnetometer calibration values: "));
  Serial.print(F("X-Axis sensitivity adjustment scale value ")); Serial.println(magCalibration[0], 2);
  Serial.print(F("Y-Axis sensitivity adjustment scale value ")); Serial.println(magCalibration[1], 2);
  Serial.print(F("Z-Axis sensitivity adjustment scale value ")); Serial.println(magCalibration[2], 2);

}

// Function which accumulates gyro and accelerometer data after device initialization. It calculates the average
// of the at-rest readings and then loads the resulting offsets into accelerometer and gyro bias registers.
// Special thanks for Kris Winers code example on github
void Compass::selfCalibrateMPU9150()
{  
  uint8_t data[12]; // data array to hold accelerometer and gyro x, y, z, data
  uint16_t ii, packet_count, fifo_count;
  int32_t gyro_bias[3]  = {0, 0, 0}, accel_bias[3] = {0, 0, 0};
  float gyroBias[3]  = {0, 0, 0}, accelBias[3] = {0, 0, 0};
  
// reset device, reset all registers, clear gyro and accelerometer bias registers
  writeByte(MPU9150_ADDRESS, MPU9150_PWR_MGMT_1, 0x80); // Write a one to bit 7 reset bit; toggle reset device
  delay(100);  
   
// get stable time source
// Set clock source to be PLL with x-axis gyroscope reference, bits 2:0 = 001
  writeByte(MPU9150_ADDRESS, MPU9150_PWR_MGMT_1, 0x01);  
  writeByte(MPU9150_ADDRESS, MPU9150_PWR_MGMT_2, 0x00); 
  delay(200);
  
// Configure device for bias calculation
  writeByte(MPU9150_ADDRESS, MPU9150_INT_ENABLE, 0x00);   // Disable all interrupts
  writeByte(MPU9150_ADDRESS, MPU9150_FIFO_EN, 0x00);      // Disable FIFO
  writeByte(MPU9150_ADDRESS, MPU9150_PWR_MGMT_1, 0x00);   // Turn on internal clock source
  writeByte(MPU9150_ADDRESS, MPU9150_I2C_MST_CTRL, 0x00); // Disable I2C master
  writeByte(MPU9150_ADDRESS, MPU9150_USER_CTRL, 0x00);    // Disable FIFO and I2C master modes
  writeByte(MPU9150_ADDRESS, MPU9150_USER_CTRL, 0x0C);    // Reset FIFO and DMP
  delay(15);
  
// Configure MPU6050 gyro and accelerometer for bias calculation
  writeByte(MPU9150_ADDRESS, MPU9150_CONFIG, 0x01);      // Set low-pass filter to 188 Hz
  writeByte(MPU9150_ADDRESS, MPU9150_SMPLRT_DIV, 0x00);  // Set sample rate to 1 kHz
  writeByte(MPU9150_ADDRESS, MPU9150_GYRO_CONFIG, 0x00);  // Set gyro full-scale to 250 degrees per second, maximum sensitivity
  writeByte(MPU9150_ADDRESS, MPU9150_ACCEL_CONFIG, 0x00); // Set accelerometer full-scale to 2 g, maximum sensitivity
 
  uint16_t  gyrosensitivity  = 131;   // = 131 LSB/degrees/sec
  uint16_t  accelsensitivity = 16384;  // = 16384 LSB/g

// Configure FIFO to capture accelerometer and gyro data for bias calculation
  writeByte(MPU9150_ADDRESS, MPU9150_USER_CTRL, 0x40);   // Enable FIFO  
  writeByte(MPU9150_ADDRESS, MPU9150_FIFO_EN, 0x78);     // Enable gyro and accelerometer sensors for FIFO  (max size 1024 bytes in MPU-6050)
  delay(80); // accumulate 80 samples in 80 milliseconds = 960 bytes

// At end of sample accumulation, turn off FIFO sensor read
  writeByte(MPU9150_ADDRESS, MPU9150_FIFO_EN, 0x00);        // Disable gyro and accelerometer sensors for FIFO
  readBytes(MPU9150_ADDRESS, MPU9150_FIFO_COUNTH, 2, &data[0]); // read FIFO sample count
  fifo_count = ((uint16_t)data[0] << 8) | data[1];
  packet_count = fifo_count/12;// How many sets of full gyro and accelerometer data for averaging

 for (ii = 0; ii < packet_count; ii++) {
    int16_t accel_temp[3] = {0, 0, 0}, gyro_temp[3] = {0, 0, 0};
    readBytes(MPU9150_ADDRESS, MPU9150_FIFO_R_W, 12, &data[0]); // read data for averaging
    accel_temp[0] = (int16_t) (((int16_t)data[0] << 8) | data[1]  ) ;  // Form signed 16-bit integer for each sample in FIFO
    accel_temp[1] = (int16_t) (((int16_t)data[2] << 8) | data[3]  ) ;
    accel_temp[2] = (int16_t) (((int16_t)data[4] << 8) | data[5]  ) ;    
    gyro_temp[0]  = (int16_t) (((int16_t)data[6] << 8) | data[7]  ) ;
    gyro_temp[1]  = (int16_t) (((int16_t)data[8] << 8) | data[9]  ) ;
    gyro_temp[2]  = (int16_t) (((int16_t)data[10] << 8) | data[11]) ;
    
    accel_bias[0] += (int32_t) accel_temp[0]; // Sum individual signed 16-bit biases to get accumulated signed 32-bit biases
    accel_bias[1] += (int32_t) accel_temp[1];
    accel_bias[2] += (int32_t) accel_temp[2];
    gyro_bias[0]  += (int32_t) gyro_temp[0];
    gyro_bias[1]  += (int32_t) gyro_temp[1];
    gyro_bias[2]  += (int32_t) gyro_temp[2];
            
    }
  accel_bias[0] /= (int32_t) packet_count; // Normalize sums to get average count biases
  accel_bias[1] /= (int32_t) packet_count;
  accel_bias[2] /= (int32_t) packet_count;
  gyro_bias[0]  /= (int32_t) packet_count;
  gyro_bias[1]  /= (int32_t) packet_count;
  gyro_bias[2]  /= (int32_t) packet_count;
    
  if(accel_bias[2] > 0L) {accel_bias[2] -= (int32_t) accelsensitivity;}  // Remove gravity from the z-axis accelerometer bias calculation
  else {accel_bias[2] += (int32_t) accelsensitivity;}
 
// Construct the gyro biases for push to the hardware gyro bias registers, which are reset to zero upon device startup
  data[0] = (-gyro_bias[0]/4  >> 8) & 0xFF; // Divide by 4 to get 32.9 LSB per deg/s to conform to expected bias input format
  data[1] = (-gyro_bias[0]/4)       & 0xFF; // Biases are additive, so change sign on calculated average gyro biases
  data[2] = (-gyro_bias[1]/4  >> 8) & 0xFF;
  data[3] = (-gyro_bias[1]/4)       & 0xFF;
  data[4] = (-gyro_bias[2]/4  >> 8) & 0xFF;
  data[5] = (-gyro_bias[2]/4)       & 0xFF;

// Push gyro biases to hardware registers for consideration in calculation
  writeByte(MPU9150_ADDRESS, MPU9150_XG_OFFS_USRH, data[0]);
  writeByte(MPU9150_ADDRESS, MPU9150_XG_OFFS_USRL, data[1]);
  writeByte(MPU9150_ADDRESS, MPU9150_YG_OFFS_USRH, data[2]);
  writeByte(MPU9150_ADDRESS, MPU9150_YG_OFFS_USRL, data[3]);
  writeByte(MPU9150_ADDRESS, MPU9150_ZG_OFFS_USRH, data[4]);
  writeByte(MPU9150_ADDRESS, MPU9150_ZG_OFFS_USRL, data[5]);

// Output scaled gyro biases for display in the main program
  gyroBias[0] = (float) gyro_bias[0]/(float) gyrosensitivity;  
  gyroBias[1] = (float) gyro_bias[1]/(float) gyrosensitivity;
  gyroBias[2] = (float) gyro_bias[2]/(float) gyrosensitivity;

// Construct the accelerometer biases for push to the hardware accelerometer bias registers. These registers contain
// factory trim values which must be added to the calculated accelerometer biases; on boot up these registers will hold
// non-zero values. In addition, bit 0 of the lower byte must be preserved since it is used for temperature
// compensation calculations. Accelerometer bias registers expect bias input as 2048 LSB per g, so that
// the accelerometer biases calculated above must be divided by 8.

  int32_t accel_bias_reg[3] = {0, 0, 0}; // A place to hold the factory accelerometer trim biases
  readBytes(MPU9150_ADDRESS, MPU9150_XA_OFFSET_H, 2, &data[0]); // Read factory accelerometer trim values
  accel_bias_reg[0] = (int16_t) ((int16_t)data[0] << 8) | data[1];
  readBytes(MPU9150_ADDRESS, MPU9150_YA_OFFSET_H, 2, &data[0]);
  accel_bias_reg[1] = (int16_t) ((int16_t)data[0] << 8) | data[1];
  readBytes(MPU9150_ADDRESS, MPU9150_ZA_OFFSET_H, 2, &data[0]);
  accel_bias_reg[2] = (int16_t) ((int16_t)data[0] << 8) | data[1];
  
  uint32_t mask = 1uL; // Define mask for temperature compensation bit 0 of lower byte of accelerometer bias registers
  uint8_t mask_bit[3] = {0, 0, 0}; // Define array to hold mask bit for each accelerometer bias axis
  
  for(ii = 0; ii < 3; ii++) {
    if(accel_bias_reg[ii] & mask) mask_bit[ii] = 0x01; // If temperature compensation bit is set, record that fact in mask_bit
  }

  // Construct total accelerometer bias, including calculated average accelerometer bias from above
  accel_bias_reg[0] -= (accel_bias[0]/8); // Subtract calculated averaged accelerometer bias scaled to 2048 LSB/g (16 g full scale)
  accel_bias_reg[1] -= (accel_bias[1]/8);
  accel_bias_reg[2] -= (accel_bias[2]/8);
 
  data[0] = (accel_bias_reg[0] >> 8) & 0xFF;
  data[1] = (accel_bias_reg[0])      & 0xFF;
  data[1] = data[1] | mask_bit[0]; // preserve temperature compensation bit when writing back to accelerometer bias registers
  data[2] = (accel_bias_reg[1] >> 8) & 0xFF;
  data[3] = (accel_bias_reg[1])      & 0xFF;
  data[3] = data[3] | mask_bit[1]; // preserve temperature compensation bit when writing back to accelerometer bias registers
  data[4] = (accel_bias_reg[2] >> 8) & 0xFF;
  data[5] = (accel_bias_reg[2])      & 0xFF;
  data[5] = data[5] | mask_bit[2]; // preserve temperature compensation bit when writing back to accelerometer bias registers

  // Push accelerometer biases to hardware registers for consideration in calculation
  writeByte(MPU9150_ADDRESS, MPU9150_XA_OFFSET_H, data[0]);
  writeByte(MPU9150_ADDRESS, MPU9150_XA_OFFSET_L_TC, data[1]);
  writeByte(MPU9150_ADDRESS, MPU9150_YA_OFFSET_H, data[2]);
  writeByte(MPU9150_ADDRESS, MPU9150_YA_OFFSET_L_TC, data[3]);
  writeByte(MPU9150_ADDRESS, MPU9150_ZA_OFFSET_H, data[4]);
  writeByte(MPU9150_ADDRESS, MPU9150_ZA_OFFSET_L_TC, data[5]);

// Output scaled accelerometer biases for display in the main program
   accelBias[0] = (float)accel_bias[0]/(float)accelsensitivity; 
   accelBias[1] = (float)accel_bias[1]/(float)accelsensitivity;
   accelBias[2] = (float)accel_bias[2]/(float)accelsensitivity;
   
// Output for user feedback
   Serial.println(F("Calibrate gyro and accelerometers and load biases in bias registers...."));
   Serial.println(F("MPU9150 acceleration bias"));
   Serial.println(F("  x\t y\t z")); Serial.print("  ");
   Serial.print((int)(1000*accelBias[0])); Serial.print(" \t");
   Serial.print((int)(1000*accelBias[1])); Serial.print(" \t");
   Serial.print((int)(1000*accelBias[2])); Serial.println(" \tmg");       
   Serial.println(F("MPU9150 gyro bias"));
   Serial.println(F("  x\t y\t z")); Serial.print("  ");
   Serial.print(gyroBias[0], 1); Serial.print(" \t");
   Serial.print(gyroBias[1], 1); Serial.print(" \t");
   Serial.print(gyroBias[2], 1); Serial.println(" \tdeg/s");      
   Serial.println(F("Calibration done"));  
}

// Accelerometer and gyroscope self test; check calibration wrt factory settings
// Special thanks for Kris Winers code example on github
void Compass::MPU6050SelfTest() // Should return percent deviation from factory trim values, +/- 14 or less deviation is a pass
{
   uint8_t rawData[4];
   uint8_t selfTest[6]; 
   float SelfTestRes[6];
   float factoryTrim[6];
   
   // Configure the accelerometer for self-test
   writeByte(MPU9150_ADDRESS, MPU9150_ACCEL_CONFIG, 0xF0); // Enable self test on all three axes and set accelerometer range to +/- 8 g
   writeByte(MPU9150_ADDRESS, MPU9150_GYRO_CONFIG,  0xE0); // Enable self test on all three axes and set gyro range to +/- 250 degrees/s
   delay(250);  // Delay a while to let the device execute the self-test
   rawData[0] = readByte(MPU9150_ADDRESS, MPU9150_SELF_TEST_X); // X-axis self-test results
   rawData[1] = readByte(MPU9150_ADDRESS, MPU9150_SELF_TEST_Y); // Y-axis self-test results
   rawData[2] = readByte(MPU9150_ADDRESS, MPU9150_SELF_TEST_Z); // Z-axis self-test results
   rawData[3] = readByte(MPU9150_ADDRESS, MPU9150_SELF_TEST_A); // Mixed-axis self-test results
   // Extract the acceleration test results first
   selfTest[0] = (rawData[0] >> 3) | (rawData[3] & 0x30) >> 4 ; // XA_TEST result is a five-bit unsigned integer
   selfTest[1] = (rawData[1] >> 3) | (rawData[3] & 0x0C) >> 4 ; // YA_TEST result is a five-bit unsigned integer
   selfTest[2] = (rawData[2] >> 3) | (rawData[3] & 0x03) >> 4 ; // ZA_TEST result is a five-bit unsigned integer
   // Extract the gyration test results first
   selfTest[3] = rawData[0]  & 0x1F ; // XG_TEST result is a five-bit unsigned integer
   selfTest[4] = rawData[1]  & 0x1F ; // YG_TEST result is a five-bit unsigned integer
   selfTest[5] = rawData[2]  & 0x1F ; // ZG_TEST result is a five-bit unsigned integer   
   // Process results to allow final comparison with factory set values
   factoryTrim[0] = (4096.0*0.34)*(pow( (0.92/0.34) , (((float)selfTest[0] - 1.0)/30.0))); // FT[Xa] factory trim calculation
   factoryTrim[1] = (4096.0*0.34)*(pow( (0.92/0.34) , (((float)selfTest[1] - 1.0)/30.0))); // FT[Ya] factory trim calculation
   factoryTrim[2] = (4096.0*0.34)*(pow( (0.92/0.34) , (((float)selfTest[2] - 1.0)/30.0))); // FT[Za] factory trim calculation
   factoryTrim[3] =  ( 25.0*131.0)*(pow( 1.046 , ((float)selfTest[3] - 1.0) ));             // FT[Xg] factory trim calculation
   factoryTrim[4] =  (-25.0*131.0)*(pow( 1.046 , ((float)selfTest[4] - 1.0) ));             // FT[Yg] factory trim calculation
   factoryTrim[5] =  ( 25.0*131.0)*(pow( 1.046 , ((float)selfTest[5] - 1.0) ));             // FT[Zg] factory trim calculation

 // Report results as a ratio of (STR - FT)/FT; the change from Factory Trim of the Self-Test Response
 // To get to percent, must multiply by 100 and subtract result from 100
   for (int i = 0; i < 6; i++) {
     SelfTestRes[i] = 100.0 + 100.0*((float)selfTest[i] - factoryTrim[i])/factoryTrim[i]; // Report percent differences
   }
      
   if(!(SelfTestRes[0] < 1.0f && SelfTestRes[1] < 1.0f && SelfTestRes[2] < 1.0f && SelfTestRes[3] < 1.0f && SelfTestRes[4] < 1.0f && SelfTestRes[5] < 1.0f)) {
     Serial.println(F("MPU6050 Selftest failed!"));
     while (1);
   }
   
}


// Function for correction of yaw angle (replaces the atan2 function for more controll)
float Compass::wrap(float x_h, float y_h){
  //TODO: Toleranz einbauen
  float microT_tol = 0.2; //within +-0.2µT the value is set to 270 or 90 deg --> no devision by or near 0 of x_h in arctan function
  float angle = 0;
  
  if(x_h < 0) angle = pi-atan(y_h/x_h);
  if(x_h > 0 && y_h < 0) angle = -atan(y_h/x_h);
  if(x_h > 0 && y_h > 0) angle = 2*pi -atan(y_h/x_h);
  //In case of small values for x_h (no devision by 0) set the angle
  if(abs(x_h) < microT_tol && y_h < 0) angle = 0.5*pi;
  if(abs(x_h) < microT_tol && y_h > 0) angle = 1.5*pi; 
  return angle;
}


// Adapted method for calibration of the magnetometer from: https://github.com/kriswiner/MPU-6050/wiki/Simple-and-Effective-Magnetometer-Calibration
// Use this method onyl from time to time in order to determine the values for the magBias[3]-vector
void Compass::calibrate( float magnetic_bias[3])
{
  uint16_t ii = 0, sample_count = 0;
  int32_t mag_bias_raw[3] = {0, 0, 0};
  int16_t mag_max[3] = {0, 0, 0}, mag_min[3] = {0, 0, 0}, mag_temp[3] = {0, 0, 0};
  delay(4000);  //Wait for 4 second to give the used some time to take the device in his hands
  sample_count = 512;
  //Gather data...this will take about 16 seconds
  for(ii = 0; ii < sample_count; ii++) {
    readMagData(mag_temp); // Read the mag data

    // Safe min max value by direct comaprision in each loop
    for (int jj = 0; jj < 3; jj++) {
      if(mag_temp[jj] > mag_max[jj]) mag_max[jj] = mag_temp[jj];
      if(mag_temp[jj] < mag_min[jj]) mag_min[jj] = mag_temp[jj];
      { Serial.print(mag_temp[jj]); Serial.print(" ");}
    }
    Serial.println();
    delay(135); // at 8 Hz ODR, new mag data is available every 125 ms
  }
  mag_bias_raw[0] = (mag_max[0] + mag_min[0])/2; // get average x mag bias in counts
  mag_bias_raw[1] = (mag_max[1] + mag_min[1])/2; // get average y mag bias in counts
  mag_bias_raw[2] = (mag_max[2] + mag_min[2])/2; // get average z mag bias in counts
  magnetic_bias[0] = (float) mag_bias_raw[0]*compRes*magCalibration[0]; // save mag biases in mG for main program
  magnetic_bias[1] = (float) mag_bias_raw[1]*compRes*magCalibration[1];
  magnetic_bias[2] = (float) mag_bias_raw[2]*compRes*magCalibration[2];
}

bool Compass::init()
{
  Wire.begin();         // Initialize the 'Wire' class for the I2C-bus.

  //*** Init the MPU9150 ***/
  // Reset the device to defaults
  writeByte(MPU9150_ADDRESS, MPU9150_PWR_MGMT_1, 0x80); delay(100);
  // Clear the 'sleep' bit to start the sensor and give the device a moment to wake up
  writeByte(MPU9150_ADDRESS, MPU9150_PWR_MGMT_1, 0x00); delay(100); 
  
  // Read the WHO_AM_I register for MPU-9150, this is a good test of communication
  uint8_t c = readByte(MPU9150_ADDRESS, MPU9150_WHO_AM_I); 
  if (c != 0x68) return false;
  
  MPU6050SelfTest();      // Self test of MPU6050 to check if all values are within factory calibration --> Make sure this method is excecuted BEFORE initMPU9150()!
  selfCalibrateMPU9150(); // Calibration of the MPU6050 chip for gyro & acc offsets --> Make sure this method is excecuted BEFORE initMPU9150()!
  //Initializing routine for MPU9150: Wakeup, Timesource, DLPF, Samplerate, Gyro & Acc range config
  initMPU9150(); //This sets the device in the desired state for normal operation

  //INIT Compass for direct access
  c = readByte(AK8975A_ADDRESS, WHO_AM_I_AK8975A); 
  if(c!=0x48) return false;
  writeByte(AK8975A_ADDRESS, AK8975A_CNTL, 0x00); // Power down
  delay(10);
  initAK8975A(magCalibration);  //Initializing routine for AK8975A: Load x,y,z-axis sensitivity values 
  return true;
}

void Compass::read()
{
  // Read all sensor values which the sensor provides and calculate phyisical values incl. corrections
  readAccelData(accRaw);
  readGyroData(gyroRaw); 
  readMagData(compRaw);
  
  // Low-Pass Filter the magnetic raw data in x, y, z since a filter later is not so easy to implement
  for(int j = 0 ;j<3;j++){
    compRaw_Filter[j] = a_CompFilter * compRaw_Filter[j] + (1-a_CompFilter) * compRaw[j];
  }
  
  for(int i = 0 ;i<3;i++){
    gyroDegPerSec[i] = gyroRaw[i] * gyroRes;
    accG[i] = accRaw[i] * accRes;
    compMicroT[i] = compRaw_Filter[i] * magCalibration[i] * compRes - magBias[i];
  }  
  

  //Safe all the main values for a better overview and for sign, direction and axis correction here
  //MPU6050 is the acc
  //The coordinate system of the Magnetometer will be used, hence PRINTED X,Y on sensor are MISLEADING --> Change x and y in mind!!!!
  ax = -accG[1];           // X-Axis is pointing FORWARD in flight direction (Aircraft nose pointing to earth = positive values) --> Invert and use y-axis of MPU6050
  ay = -accG[0];           // Y-Axis is pointing to the RIGHT side of the AC (right wing is pointing to earth = positive values) --> Invert and use x-axis of MPU6050
  az = accG[2];            // Z-Axis is pointing DOWNWARDS (z-axis pointing to earth = positive values) --> OK
  gx = gyroDegPerSec[1];   // Rotation around x-axis is ROLL and clockwise positive/a roll to the right is positive --> Use y-axis of MPU6050
  gy = gyroDegPerSec[0];   // Rotation around y-axis is PITCH and clockwise positive/nose up is positive --> Use x-axis of MPU6050
  gz = -gyroDegPerSec[2];  // Rotation around z-axis is YAW and clockwise positive/a turn to east is positive --> Invert for MPU6050
  mx = compMicroT[0];      // x-axis to north = positive values / to east/west = near 0 / to south = negative --> OK
  my = compMicroT[1];      // y-axis to north = positive values / to east/west = near 0 / to south = negative --> OK
  mz = compMicroT[2];      // if sensor is leveled = positive values -->OK
  //Serial.print(ax);Serial.print("\t");Serial.print(ay);Serial.print("\t");Serial.print(az);Serial.print("\t");Serial.print(gx);Serial.print("\t");Serial.print(gy);Serial.print("\t");Serial.print(gz);Serial.print("\t | ");Serial.print(mx);Serial.print("\t");Serial.print(my);Serial.print("\t");Serial.print(mz);Serial.print("\t");Serial.print(mx_h);Serial.print("\t");Serial.print(my_h);Serial.println();
  
  /*** Angle Calculation and Sensor Fusion ***/
  //Angle calculation from accelerometer. x-Axis pointing to front (ATTENTION: IS THE PRINTED Y-AXIS ON THE SENSOR!)
  pitchAcc = atan2(ax, az); // pitch is angle between x-axis and z-axis
  pitchAcc = -pitchAcc * RadToDeg;      // pitch is positiv upwards (climb) --> Invert the angle! 
  
  rollAcc = atan2(ay, az); // roll is angle between y-axis and z-axis
  rollAcc = rollAcc * RadToDeg;         // roll is clockwise positiv to the right (rotation clockwise in flight direction) 
  
  // Calculation of the heading

  // With tilt compensation
  // Takes raw magnetometer values, pitch and roll and turns it into a tilt-compensated heading value ranging from -pi to pi (everything in this function should be in radians).
  // Basically we first "unturn" the roll-angle and then the pitch to have mx and my in the horizontal plane again
  pitchRad = -m_pitch/RadToDeg;  //angles have to be inverted again for unturing from actual plane to horizontal plane
  rollRad = -m_roll/RadToDeg;
  mx_h = mx*cos(pitchRad) + my*sin(rollRad)*sin(pitchRad) - mz*cos(rollRad)*sin(pitchRad);
  my_h = my*cos(rollRad) + mz*sin(rollRad);
  yawMag = wrap(mx_h, my_h);
  //yawMag = wrap(mx,my);  //without tilt compensation
  //Serial.print(mx);Serial.print("\t");Serial.print(my);Serial.print("\t");Serial.print(mx_h);Serial.print("\t");Serial.println(my_h);Serial.println();

  yawMag = yawMag * RadToDeg;
  //yawMag = yawMag + declinationAngle; // Subtracking the 'Declination Angle' in Deg --> a positive (to east) declination angle has to be subtracked
  //float inclinationAngle = atan(mz/sqrt(mx*mx+my*my))* RadToDeg;
  //Serial.print("Inclination Angle = ");Serial.println(inclinationAngle);
  
  
  //Sensor fusion
  now = micros();
  dT = (now - lastupdate)/1000000.0;
  lastupdate=micros();
  
  m_pitch = a_coeff * (m_pitch + gy * dT) + (1-a_coeff) * pitchAcc;  // pitch is the rotation around the y-axis  
  m_roll  = a_coeff * (m_roll  + gx * dT) + (1-a_coeff) * rollAcc;  // roll is the rotation around the x-axis
  // yaw is the rotation around the z-axis. ATTENTION: Simple filtering as for pitch and roll does not work here properly due to the change betwenn 0° - 360° and vice versa
  //       e.g "yaw = a_coeff * yaw + (1-a_coeff) * yawMag;" does not work
  m_yaw = yawMag;
  if(m_yaw>360) m_yaw = m_yaw-360; //care for the change from 0-->360 or 360-->0 near to north
  if(m_yaw<0) m_yaw = m_yaw+360;

  Sensor::read();
}

} // end namespace

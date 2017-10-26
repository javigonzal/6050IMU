/* --MPU 6050--
 Hardware setup:
 MPU6050 Breakout --------- Arduino
 VDD ---------------------- 3.3V
 VDDI --------------------- 3.3V
 SDA ----------------------- A4
 SCL ----------------------- A5
 GND ---------------------- GND
 */
#include "MPU6050.h"

#define AHRS true         // Set to false for basic data read
#define SerialDebug true  

MPU6050 myIMU;

void setup(void) 
{
  Wire.begin();
  Serial.begin(38400);
  byte c = myIMU.readByte(MPU6050_ADDRESS, WHO_AM_I_MPU6050);
  if (c == 0x68) // WHO_AM_I should always be 0x68{  
    Serial.println("MPU6050 is online...");
    
    myIMU.MPU6050SelfTest(SelfTest); 
    if(SelfTest[0] < 1.0f && SelfTest[1] < 1.0f && SelfTest[2] < 1.0f && SelfTest[3] < 1.0f && SelfTest[4] < 1.0f && SelfTest[5] < 1.0f) {
      myIMU.calibrateMPU6050(gyroBias, accelBias); // Calibrate gyro and accelerometers, load biases in bias registers
      myIMU.initMPU6050(); 
      Serial.println("MPU6050 initialized for active data mode...."); // Initialize device for active mode read of acclerometer, gyroscope, and temperature
   }else{
    Serial.print("Could not connect to MPU6050: 0x");
    Serial.println(c, HEX);
    while(1) ; // Loop forever if communication doesn't happen
   }
}
void loop()
{
  if (myIMU.readByte(MPU6050_ADDRESS, INT_STATUS) & 0x01){
    myIMU.readAccelData(accelCount);
    myIMU.getAres();

    ax = (float)accelCount[0] * aRes; // - myIMU.accelBias[0];
    ay = (float)accelCount[1] * aRes; // - myIMU.accelBias[1];
    az = (float)accelCount[2] * aRes; // - myIMU.accelBias[2];

    myIMU.readGyroData(gyroCount);
    myIMU.getGres();// Read the x/y/z adc values

    // Calculate the gyro value into actual degrees per second
    // This depends on scale being set
    gx = (float)gyroCount[0] * gRes;
    gy = (float)gyroCount[1] * gRes;
    gz = (float)gyroCount[2] * gRes;
  }

    //  myIMU.updateTime();
    Now = micros();
    deltat = ((Now - lastUpdate)/1000000.0f); // set integration time by time elapsed since last filter update
    lastUpdate = Now;

  myIMU.MadgwickQuaternionUpdate(ax, ay, az, gx*PI/180.0f, gy*PI/180.0f, gz*PI/180.0f);
  Serial.print("qparent\t");
  Serial.print(q[1], 2);
  Serial.print("\t");
  Serial.print(q[3], 2);
  Serial.print("\t");
  Serial.print(q[2], 2);
  Serial.print("\t");
  Serial.println(-q[0], 2);
}

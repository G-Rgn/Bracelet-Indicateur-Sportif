/*
* This file contain the SPO2 function that allow the bracelet to get a SPO2 value (%)
* from the optical sensor MAX30102. The function is running only once as the value
* of the oxygen rate does not variate very much.
* Note : an optimal value of oxygen rate is arround 98%.
*/

#ifndef _SPO2_H_
#define _SPO2_H_


/****************************/
/********** FILES ***********/
/****************************/

#include "MAX30105.h"
#include "spo2_algorithm.h"


/****************************/
/****** PIN DEFINITION ******/
/****************************/

#define I2C_SDA_PIN         15                                                   
#define I2C_SCL_PIN         13                                                  
                                                                                

/****************************/
/* VARIABLES INITIALISATION */
/****************************/

MAX30105 particleSensor2;                                                       // Set the sensor as particleSensor2

uint32_t irBuffer[100];                                                         // Infrared LED sensor data
uint32_t redBuffer[100];                                                        // Red LED sensor data

int32_t bufferLength;                                                           // Data length
int32_t spo2;                                                                   // SPO2 value
int8_t validSPO2;                                                               // Indicator to show if the SPO2 calculation is valid
int32_t heartRate;                                                              // HeartRate value
int8_t validHeartRate;                                                          // Indicator to show if the HeartRate calculation is valid

byte pulseLED = 11;                                                             // Must be on PWM pin
byte readLED = 13;                                                              // Blinks with each data read


/****************************/
/******** FUNCTION  *********/
/****************************/

// Function that return the SPO2 value
int mesureSpo2(){
  
  Wire1.begin(I2C_SDA_PIN, I2C_SCL_PIN);                                        // Begin the wire connection

  // Initialize sensor
  while(!particleSensor2.begin(Wire1, 400000)){                                  // Use default I2C port, 400kHz speed
    return -1;                                                                  // Return -1 if the sensor is not found
  }

  pinMode(pulseLED, OUTPUT);                                                    // Set the IR LED as output
  pinMode(readLED, OUTPUT);                                                     // Set the Red LED as output

  byte ledBrightness = 60;                                                      // Options: 0=Off to 255=50mA
  byte sampleAverage = 4;                                                       // Options: 1, 2, 4, 8, 16, 32
  byte ledMode = 2;                                                             // Options: 1 = Red only, 2 = Red + IR, 3 = Red + IR + Green
  byte sampleRate = 100;                                                        // Options: 50, 100, 200, 400, 800, 1000, 1600, 3200
  int pulseWidth = 411;                                                         // Options: 69, 118, 215, 411
  int adcRange = 4096;                                                          // Options: 2048, 4096, 8192, 16384

  particleSensor2.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); // Configure sensor with these settings

  bufferLength = 100;                                                           // Buffer length of 100 stores 4 seconds of samples running at 25sps

  // Read the first 100 samples, and determine the signal range
  for (byte i = 0 ; i < bufferLength ; i++){
    while (particleSensor2.available() == false)                                // Do we have new data?
      particleSensor2.check();                                                  // Check the sensor for new data

    redBuffer[i] = particleSensor2.getRed();                                    // Get Red LED value
    irBuffer[i] = particleSensor2.getIR();                                      // Get IR LED value
    particleSensor2.nextSample();                                               // Move to next sample
  }

  // Calculate SpO2 after first 100 samples (first 4 seconds of samples)
  maxim_heart_rate_and_oxygen_saturation(irBuffer, bufferLength, redBuffer, &spo2, &validSPO2, &heartRate, &validHeartRate);

  ledBrightness = 0;                                                            // Set LED brightness to 0
  particleSensor2.setup(ledBrightness, sampleAverage, ledMode, sampleRate, pulseWidth, adcRange); // Configure sensor with these settings
  delay(100);                                                                   // Add a little delay of 100ms

  if(validSPO2 == 1){                                                           // Check if the SPO2 value is valid
          return spo2;                                                          // Return the SPO2 value
      }
}

#endif _SPO2_H_

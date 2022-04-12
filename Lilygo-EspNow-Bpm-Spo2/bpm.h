/*
* This file contain the BPM function that allow the bracelet to get a beat per minute value
* from the optical sensor MAX30102. The function can run infinitely as the value
* of the beat per minute does variate a lot during time.
* Note : an normal value of bpm (resting) is between 55 and 100 beat per minute.
* Note 2 : the maximum value (in full effort) is equal to 220 - age of the user.
*/

#ifndef _BPM_H_
#define _BPM_H_


/****************************/
/********** FILES ***********/
/****************************/

#include "MAX30105.h"
#include "heartRate.h"


/****************************/
/****** PIN DEFINITION ******/
/****************************/

#define HEATRATE_SDA        15
#define HEATRATE_SCL        13
#define HEATRATE_INT        4


/****************************/
/* VARIABLES INITIALISATION */
/****************************/

MAX30105 particleSensor;                                                        // Set the sensor as particleSensor

const uint8_t RATE_SIZE = 4;                                                    // Increase this for more averaging. 4 is good.
uint8_t rates[RATE_SIZE];                                                       // Array of heart rates
uint8_t rateSpot = 0;
long lastBeat = 0;                                                              // Time at which the last beat occurred
float beatsPerMinute;                                                           // Variable that will count the beats per minute


/****************************/
/******** FUNCTION  *********/
/****************************/
// Function that return the BPM value
int BPM()
{
    Wire1.begin(HEATRATE_SDA, HEATRATE_SCL);                                    // Begin the wire connection
    // Initialize sensor
    if (!particleSensor.begin(Wire1, 400000)) {                                 // Use default I2C port, 400kHz speed
        return -1;                                                              // Return -1 if the sensor is not found
    }
    else{
       particleSensor.setup();                                                  // Configure sensor with default settings
       particleSensor.setPulseAmplitudeRed(0);                                  // Turn Red LED to low to indicate sensor is running 0x0A
    
       long irValue = particleSensor.getIR();                                   // Get the value of IR LED
  
       int beatAvg = 0;                                                         // Set the beat average variable to 0

         if (checkForBeat(irValue) == true) {                                   // Check for a beat
             long delta = millis() - lastBeat;                                  // Set a delta witch is current time minus last beat
             lastBeat = millis();                                               // Set last beat to current time
      
             beatsPerMinute = 60 / (delta / 1000.0);                            // Calculation for the beats per minute
              
             if (beatsPerMinute < 255 && beatsPerMinute > 20) {                 // Check the quality of values
                 rates[rateSpot++] = (uint8_t)beatsPerMinute;                   // Store this reading in the array
                 rateSpot %= RATE_SIZE;                                         // Wrap variable
      
                 // Take average of readings
                 beatAvg = 0;
                 for (uint8_t x = 0 ; x < RATE_SIZE ; x++)                      // Get an average
                     beatAvg += rates[x];                                       // Add values to the beat average variable
                 beatAvg /= RATE_SIZE;                                          // Get a  of the BPM
             }
         }     
      return beatAvg;                                                           // Return the beat average
    }
}
#endif _BPM_H_

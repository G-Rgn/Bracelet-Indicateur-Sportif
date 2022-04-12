# LilyGo_ESPNOW

### Read before use

The goal of this project is to establish a communication between a LilyGo bracelet (slave) and an ESP32 (master).  
The LilyGo can measure heart rate as well as oxygenation levels in the blood. The ESP master can order a remote measurement via the ESP-NOW protocol.  
In the file `Lilygo-EspNow-Bpm-Spo2` you will find the steps to follow and the codes to make the LilyGo bracelet work.
In the second file `Master-ESP32`, we provide the code for the ESP master which works with a button.
Everything is explained and commented in order to facilitate the reproduction of this project.


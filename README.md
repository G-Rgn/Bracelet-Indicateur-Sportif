# Read before use

The goal of this project is to establish a communication between a LilyGo bracelet (slave) and an ESP32 (master).
The LilyGo can measure heart rate as well as oxygenation levels in the blood. The ESP master can order a remote measurement via the ESP-NOW protocol.  
In the file `Lilygo-EspNow-Bpm-Spo2` you will find the steps to follow and the codes to make the LilyGo bracelet work.  
In the second file `Master-ESP32`, we provide the code for the ESP master which works with a button.  
Everything is explained and commented in order to facilitate the reproduction of this project.

## Installation process
**This step is mandatory and commune for the next parts**
1. Download Arduino IDE
2. Add the ESP32 packages
    - Go to File > Preferences and add `https://dl.espressif.com/dl/package_esp32_index.json`  
    ![arduino1](https://user-images.githubusercontent.com/103428967/162929793-0f206ff7-50c5-46e0-b38b-08fba3cb1b58.png)  
3. Follow the tutorials on both `LilyGo-EspNow-Bpm-Spo2` and `Master-ESP` files

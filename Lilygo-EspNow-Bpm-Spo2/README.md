#  Arduino LilyGo setup  

### Informations
Through this manual, we will trace the steps that allow us to quickly arrive at the same result as the product we have. This is a tutorial that is meant to be as detailed and efficient as possible in order to avoid wasting any time in reworking the project.  
This work is the result of an adaptation and improvement of existing code. We started from the version of the code of the product [LILYGO®TTGO](https://github.com/Xinyuan-LilyGO/LilyGo-T-Wristband) as well as those of the sensor MAX30102.  
It should be noted that this sensor does not have a green LED, which impacts the measurement of heart rate. This may be an important part for future improvement.  
For any information that could not find its source here, do not hesitate to consult the codes that have served as a starting point.

### Installation process
1. Download Arduino IDE and set the ESP32 in your preferences as it is mentionned in the first [README](https://github.com/G-Rgn/Bracelet-Indicateur-Sportif)
  
2. Dowload the driver  
    - [CP21xx Drivers](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers)  
  
3. Add librairies to Arduino IDE
    - Go to the GitHub of the library  
    - Click on " Code " then " Download ZIP "  
    - Download the folder in your Arduino space  
  
    ![library](https://user-images.githubusercontent.com/103428967/162743352-12e163d3-4096-4c08-a48c-40cfe7aa9c40.png)  
  
    - In the context of this project, the following libraries may be useful:  
      - [TFT_eSPI](https://github.com/Bodmer/TFT_eSPI)  
      - [SparkFun](https://github.com/sparkfun/SparkFun_LSM9DS1_Arduino_Library)  
      - [PCF8563_Library](https://github.com/lewisxhe/PCF8563_Library)  

    - To add it in the Arduino IDE, sketch > include a library > add to library.ZIP then select the .ZIP folder.  
    
   ![arduino](https://user-images.githubusercontent.com/103428967/162925298-8279f7a3-b1ed-4dcd-89ca-db82edb6d662.png)  
   
4. Final step
    - Download the files from this GitHub
    - Open it on your IDE
    - Connect your LilyGo to the arduino and the arduino to the computer
    - Select the good port
    - Upload the code

5. When a touch is detected, it will change the mode

    - Press for the first time to calculate the BPM
    - Press it a second time to calculate SPO2
    - Press it during 3 seconds to enter in deep sleep
    - During deep sleep, touch again to wake up the bracelet

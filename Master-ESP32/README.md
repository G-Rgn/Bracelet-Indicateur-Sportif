# ESPNOW setup

### Informations
To establish the communication between the two devices using the ESPNOW protocol, we were able to draw inspiration from several sites, including these:
  - [Espressif](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/network/esp_now.html)
  - [Random Nerd Turorials](https://randomnerdtutorials.com/esp-now-esp32-arduino-ide/)  
  
All the information you may need about the protocol and its use can be found on the internet.
It is important to note that in the codes provided, **the MAC addresses will have to be modified** to match those of your devices. To do this, you can use the `MacAdress` script.

### Installation process
1. Download Arduino IDE and set the ESP32 in your preferences as it is mentionned in the first `README`
2. Find the MAC Adress of your devices :
    - Download the `MacAdress` script
    - Run it on all your device (master and slave)
    - Write those adresses somewhere, you will need them later
4. Download the `Master-ESP`script
5. Replace the Mac Adress line 21 with the one you find in the previous part 2 (see image1 below)
6. Upload the code to your master ESP32 by choosing the good port

### Mounting with the button
The code is written in such a way that when the user presses the button, it sends a message to the slave (in this case the LilyGo bracelet) to request heart rate and blood oxygen values. Here is the step to get the same assembly.
 
1. Get all the following material:
    - ESP32 (x1)
    - Wires (x4)
    - Button (x1)
    - Breadboard (x1)
2. Put the ESP and the buton on the breadboard
3. Follow the diagrams below for the installation of the wires
4. Adjust the code (line 24) if your buton isn't set on the pin 22  
  
![Image1](https://user-images.githubusercontent.com/103428967/162972124-e1a6ab04-90ca-44b2-9bd2-a3c3e4be9ebe.png)  
<p align="center">Image 1 (above)</p>  
  
  
![assembly](https://user-images.githubusercontent.com/103428967/162979987-2b3e5fac-1b02-4340-a2bd-19c390f4c9af.png)
<p align="center">Assembly diagram (above)</p>  
  
  
![Image1](https://user-images.githubusercontent.com/103428967/162970406-de1ad2a5-8d0d-4c6a-b4da-f8db659338b3.jpg) 
<p align="center">Assembly photo (above)</p>  

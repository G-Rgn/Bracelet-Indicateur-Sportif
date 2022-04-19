> Solution to use watches on the market without knowing how they work at first.  
# Understand how Bluetooth communication works between the phone and the watch
## Method with the BLEFriend sniffer
> Flash to be done if the product is not in sniffer format  
> [Perform Flash](https://github.com/G-Rgn/Bracelet-Indicateur-Sportif/blob/ConnectESP32/flashcartesniffer.txt).
  
Install the module according to the steps indicated in the [Installation](https://github.com/G-Rgn/Bracelet-Indicateur-Sportif/blob/ConnectESP32/installationSniffer.txt).  
Once the installation is done, you can retrieve all the data that has passed between the device that is traced and the outside.  
### Data to exploit
The most interesting data are the Bluetooth service that is requested, as well as the messages sent and received.  
The Bluetooth connection and the creation of the GATT server is done automatically thanks to the [NimBLE-Arduino] library (https://github.com/h2zero/NimBLE-Arduino).  
The frames from the communication will allow to learn how the phone communicates with the watch and thus reproduce the same behavior with the ESP32.  
## Method with HCI mode on an Android smartphone
### Mandatory point
Activate the developer mode on your Android phone (Settings > About > Software information > Click several times on the Version number
until your phone tells you to enable this mode).  
You may potentially need to download adb on your computer. [ADB download link](https://dl.google.com/android/repository/platform-tools-latest-windows.zip)  
### Recovering the communication logs
Please follow the procedure indicated in the corresponding file to get the [HCI logs](https://github.com/G-Rgn/Bracelet-Indicateur-Sportif/blob/ConnectESP32/hciBluetooth.txt).  
Once the file is recovered, open it with Wireshark.  
## Study of the application reserved for the watch model
1. Download the desired application in APK format
2. Download [JADX](https://www.softpedia.com/dyn-postdownload.php/a5035ed4b84749d8d2d91517a3eabd3c/62569f05/38cdc/0/1)
3. Open the application with the software
4. Find functions that may be of interest, including using the search tool

# Using the information collected
Thanks to all the possibilities offered to understand the functioning of your connected watch/wristband, you can now simulate your phone with the ESP321.  
You will find in this repository all the codes that allow to communicate with the watches of our project (SB7+ and SB18+) and you will be able to transpose it for your watch model and your protocols.

> Solution to use watches on the market without knowing how they work at first.  
# Understand how Bluetooth communication works between the phone and the watch
## Method with the BLEFriend sniffer
> Flash to be done if the product is not in sniffer format  
> [Perform Flash](https://github.com/G-Rgn/Bracelet-Indicateur-Sportif/tree/ConnectESP32/Flash_NRF51)  
>   
> Branchement du ST Link  
>   
> ![IMG_20180726_121308-169x300](https://user-images.githubusercontent.com/71140515/164702079-e4ad2e67-f073-44dc-91f1-7bd02a7f8d86.jpg) ![IMG_20180726_121408-300x169](https://user-images.githubusercontent.com/71140515/164701891-5b72366f-4b68-45bf-949a-9f2a029b5ad8.jpg)  
> Ground (Blue)
Target Voltage for sensing (Green)
SWCLK (Purple)
SWDIO (Grey)  
  
> To check if the flashing is correctly done, your Sniffer must blink with the Blue LED

  
Install the module according to the steps indicated in the [Installation](https://github.com/G-Rgn/Bracelet-Indicateur-Sportif/blob/ConnectESP32/installationSniffer.txt).  

Once the installation is done, you can retrieve all the data that has passed between the device that is traced and the outside.   
  
![Capture d’écran trame](https://user-images.githubusercontent.com/71140515/164703493-ea2b8237-56b7-4523-ac54-d78a7dcfa256.png)
> Exemple de trame trouvée  

### Data to exploit
The most interesting data are the Bluetooth service that is requested, as well as the messages sent and received.  
The Bluetooth connection and the creation of the GATT server is done automatically thanks to the [NimBLE-Arduino library](https://github.com/h2zero/NimBLE-Arduino).  
The frames from the communication will allow to learn how the phone communicates with the watch and thus reproduce the same behavior with the ESP32.  
## Method with HCI mode on an Android smartphone
### Mandatory point
Activate the developer mode on your Android phone (Settings > About > Software information > Click several times on the Version number
until your phone tells you this mode is enabled).  
![how-to-enable-developer-options-screenshot-03-2-153x339](https://user-images.githubusercontent.com/71140515/164703824-1eb295de-897d-4b8a-b724-5f1891a69f4f.jpg)

You may potentially need to download adb on your computer to find the file. [ADB download link](https://dl.google.com/android/repository/platform-tools-latest-windows.zip)  
### Recovering the communication logs
Please follow the procedure indicated in the corresponding file to get the [HCI logs](https://github.com/G-Rgn/Bracelet-Indicateur-Sportif/blob/ConnectESP32/hciBluetooth.txt).  
Once the file is recovered, open it with Wireshark.  
## Study of the application reserved for the watch model
1. Download the desired application in APK format
2. Download [JADX](https://www.softpedia.com/dyn-postdownload.php/a5035ed4b84749d8d2d91517a3eabd3c/62569f05/38cdc/0/1)
3. Open the application with the software
4. Find functions that may be of interest, using the search tool may help  
![jadxscreen](https://user-images.githubusercontent.com/71140515/164704292-ca937788-fd67-4bb8-aaa9-99f33de0866c.png)

# Using the information collected
Thanks to all the possibilities offered to understand the functioning of your connected watch/wristband, you can now simulate your phone with the ESP32.  
You will find in this repository all the codes that allow to communicate with the watches of our project (SB7+ and SB18+) and you will be able to transpose it for your watch model and your protocols.

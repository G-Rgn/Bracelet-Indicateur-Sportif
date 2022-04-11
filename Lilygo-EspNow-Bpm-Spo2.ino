/*
* This program is a variant of the Lilygo connected bracelet code.
* The source code of which is available at the following address:
* https://github.com/Xinyuan-LilyGO/LilyGo-T-Wristband.git
* The purpose of this code is to allow a master ESP32 to control the measurement
* (heartbeat and blood oxygenation rate) and retrieve these values 
* via the ESP-NOW protocol. With the bracelet, we use a MAX30102 optical sensor 
* whose libraries and source code are also available on github.
* It should be noted that this type of sensor does not include a green LED,
* which impacts the quality and accuracy of the spo2 measurement.
*/


/****************************/
/******** LIBRARIES *********/
/****************************/

#include <pcf8563.h>
#include <TFT_eSPI.h>
#include <SPI.h>
#include <Wire.h>
#include <WiFi.h>
#include <esp_now.h>
#include <WiFiManager.h>


/****************************/
/********** FILES ***********/
/****************************/

#include "sensor.h"
#include "esp_adc_cal.h"
#include "ttgo.h"
#include "charge.h"
#include "bpm.h"
#include "spo2.h"

//#define ARDUINO_OTA_UPDATE      //! Enable this line OTA update

#ifdef ARDUINO_OTA_UPDATE
#include <ESPmDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#endif


/****************************/
/****** PIN DEFINITION ******/
/****************************/

#define LED_PIN             4
#define TP_PIN_PIN          33
#define I2C_SDA_PIN         21
#define I2C_SCL_PIN         22
#define IMU_INT_PIN         38
#define RTC_INT_PIN         34
#define BATT_ADC_PIN        35
#define VBUS_PIN            36
#define TP_PWR_PIN          25
#define CHARGE_PIN          32



/****************************/
/* VARIABLES INITIALISATION */
/****************************/

// Basic initializations of variables useful in the program
extern MPU9250 IMU;
PCF8563_Class rtc;
WiFiManager wifiManager;
TFT_eSPI tft = TFT_eSPI();
bool rtcIrq = false;
bool initial = 1;
bool otaStart = false;
uint8_t func_select = 0;
uint8_t xcolon = 0;
int vref = 1100;
bool pressed = false;
bool charge_indication = false;

// Variables for the heartbeat mesure
int value_bpm;
int j = 0;
int moy;

// Variables for the oxygen rate mesure
int vspo2;
int prise_spo2 = 0;

// Buffer for the display on the screen
char buff[256];

// Time variables
uint32_t currentTime;
uint32_t pressedTime = 0;
uint32_t targetTime = 0;
uint8_t hh, mm, ss ;
uint8_t omm = 99;

// Structure use in the ESP-NOW protocol, (must match the receiver structure)
const char nom[10]="Slave0";                                                    // Name of the bracelet
uint8_t broadcastAddress[] = {0x24, 0x6F, 0x28, 0x7B, 0xF1, 0xFC};              // Master MAC Address

typedef struct struct_message {
  char a[32];                                                                   // Name of the transmitter
  int bpm;                                                                      // Heartbeat value
  int spo2;                                                                     // Oxygen rate value
} struct_message;
struct_message dataSent;
struct_message dataRcv;


/****************************/
/******** FUNCTIONS  ********/
/****************************/

// Callbacks for sending data
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nSlave0 packet sent:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail"); // Print the state of the delivery
}

// Procedure to adopt when the master ask for mesurements
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&dataRcv, incomingData, sizeof(dataRcv));
  Serial.print("From ");
  Serial.println(dataRcv.a);                                                    // Print the name of the transmitter of the message
  Serial.println("actions : ");                                                 // If actions are both -1, the slave send back measurements of BPM and SPO2
  Serial.println(dataRcv.bpm);
  Serial.println(dataRcv.spo2);

  // Check that the master send -1 both for BPM and SPO2
  if (dataRcv.bpm == -1 && dataRcv.spo2 == -1)
  {
    // Initialization of variables
    int i = 0;
    int moyenne = 0;
    int value = 0;
    int send_spo2 = 0;

    // Enter a while loop that end only when the message is send from the slave
    while (true)
    {
      value = BPM();                                                            // Launch of the BPM function
      if (value > 40) {                                                         // If the value is greater than 40, the program continues
        moyenne += value;                                                       // The value is added to the previous ones
        i++;                                                                    // Incrementing a counter variable
        if(i == 4){                                                             // We need 4 values of BPM to continu
          moyenne /= 4;                                                         // We create a average of Heartbeat with those 4 values
          send_spo2 = mesureSpo2();                                             // Then we start the spo2 mesurement
          if(send_spo2<=100 && send_spo2>0){                                    // We check the quality of the data
            strcpy(dataSent.a, nom);                                            // We start the procedure for sending the message by giving the name
            dataSent.bpm = moyenne;                                             // We add the value of heartbeat
            dataSent.spo2 = send_spo2;                                          // And the one of oxygen rate
            esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &dataSent, sizeof(dataSent)); // The message is send to the master
            delay(1000);                                                        // We wait for the message to be send
            if(result == ESP_OK){                                               // If the message is send we print a message on the serial monitor
              Serial.println("Send successful");
              break;
            }
          }
        }
      }
    }
  }
}

/**************************************************************************************************************************************/
/**** No need of this part for our code but we chose to keep the oortunity for the user to activate it for a futur need in his use ****/
/**************************************************************************************************************************************/
void configModeCallback (WiFiManager *myWiFiManager){
    Serial.println("Entered config mode");
    Serial.println(WiFi.softAPIP());
    //if you used auto generated SSID, print it
    Serial.println(myWiFiManager->getConfigPortalSSID());

    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.drawString("Connect hotspot name ",  20, tft.height() / 2 - 20);
    tft.drawString("configure wrist",  35, tft.height() / 2  + 20);
    tft.setTextColor(TFT_GREEN);
    tft.drawString("\"T-Wristband\"",  40, tft.height() / 2 );

}

void drawProgressBar(uint16_t x0, uint16_t y0, uint16_t w, uint16_t h, uint8_t percentage, uint16_t frameColor, uint16_t barColor){
    if (percentage == 0) {
        tft.fillRoundRect(x0, y0, w, h, 3, TFT_BLACK);
    }
    uint8_t margin = 2;
    uint16_t barHeight = h - 2 * margin;
    uint16_t barWidth = w - 2 * margin;
    tft.drawRoundRect(x0, y0, w, h, 3, frameColor);
    tft.fillRect(x0 + margin, y0 + margin, barWidth * percentage / 100.0, barHeight, barColor);
}

void setupWiFi(){
#ifdef ARDUINO_OTA_UPDATE
    WiFiManager wifiManager;
    //set callback that gets called when connecting to previous WiFi fails, and enters Access Point mode
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setBreakAfterConfig(true);                                      // Without this saveConfigCallback does not get fired
    wifiManager.autoConnect("T-Wristband");
#endif
}

void setupOTA(){
#ifdef ARDUINO_OTA_UPDATE
    // Port defaults to 3232
    // ArduinoOTA.setPort(3232);

    // Hostname defaults to esp3232-[MAC]
    ArduinoOTA.setHostname("T-Wristband");

    // No authentication by default
    // ArduinoOTA.setPassword("admin");

    // Password can be set with it's md5 value as well
    // MD5(admin) = 21232f297a57a5a743894a0e4a801fc3
    // ArduinoOTA.setPasswordHash("21232f297a57a5a743894a0e4a801fc3");

    ArduinoOTA.onStart([]() {
        String type;
        if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
        else // U_SPIFFS
            type = "filesystem";

        // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
        Serial.println("Start updating " + type);
        otaStart = true;
        tft.fillScreen(TFT_BLACK);
        tft.drawString("Updating...", tft.width() / 2 - 20, 55 );
    })
    .onEnd([]() {
        Serial.println("\nEnd");
        delay(500);
    })
    .onProgress([](unsigned int progress, unsigned int total) {
        // Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        int percentage = (progress / (total / 100));
        tft.setTextDatum(TC_DATUM);
        tft.setTextPadding(tft.textWidth(" 888% "));
        tft.drawString(String(percentage) + "%", 145, 35);
        drawProgressBar(10, 30, 120, 15, percentage, TFT_WHITE, TFT_BLUE);
    })
    .onError([](ota_error_t error) {
        Serial.printf("Error[%u]: ", error);
        if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
        else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
        else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
        else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
        else if (error == OTA_END_ERROR) Serial.println("End Failed");

        tft.fillScreen(TFT_BLACK);
        tft.drawString("Update Failed", tft.width() / 2 - 20, 55 );
        delay(3000);
        otaStart = false;
        initial = 1;
        targetTime = millis() + 1000;
        tft.fillScreen(TFT_BLACK);
        tft.setTextDatum(TL_DATUM);
        omm = 99;
    });

    ArduinoOTA.begin();
#endif
}
/**************************************************************************************************************************************/
/**************************************************************************************************************************************/
/**************************************************************************************************************************************/

// Function that allow the setup of the ADC
void setupADC(){
    esp_adc_cal_characteristics_t adc_chars;                                    // Get characteristics of the ADC converter
    esp_adc_cal_value_t val_type = esp_adc_cal_characterize((adc_unit_t)ADC_UNIT_1, (adc_atten_t)ADC1_CHANNEL_6, (adc_bits_width_t)ADC_WIDTH_BIT_12, 1100, &adc_chars);
    //Check type of calibration value used to characterize ADC
    if (val_type == ESP_ADC_CAL_VAL_EFUSE_VREF) {                               // Calibration of the ADC by voltage reference
        Serial.printf("eFuse Vref:%u mV", adc_chars.vref);                      // Print the default value of voltage
        Serial.println();
        vref = adc_chars.vref;                                                  // Give the value of the reference voltage of ADC
    } else if (val_type == ESP_ADC_CAL_VAL_EFUSE_TP) {                          // Calibration of ADC by two points
        Serial.printf("Two Point --> coeff_a:%umV coeff_b:%umV\n", adc_chars.coeff_a, adc_chars.coeff_b);   // Print the value of the two coefficient
    } else {
        Serial.println("Default Vref: 1100mV");                                 // Print the default value of voltage
    }
}

// Function use to set the time and calendar up
void setupRTC(){
    rtc.begin(Wire);                                                            // Begin the connection of the rtc module
    rtc.check();                                                                // Check if the RTC clock matches, if not, use compile time

    RTC_Date datetime = rtc.getDateTime();                                      // Get the date and the time, then define variables for :
    hh = datetime.hour;                                                         // Hours
    mm = datetime.minute;                                                       // Minutes
    ss = datetime.second;                                                       // Seconds
}

// Function use to set the screen up
void screenSetup(){
    tft.fillScreen(TFT_DARKGREY);                                               // Set the background color up
    tft.setTextColor(TFT_YELLOW, TFT_DARKGREY);                                 // Set the color of the font and the background

    targetTime = millis() + 1000;                                               // Use a time variable to

    pinMode(TP_PIN_PIN, INPUT);                                                 // Configure the TP_PIN_PIN as an input, it represent the touch of the button
    pinMode(TP_PWR_PIN, PULLUP);                                                // Configure the TP_PWR_PIN as an pullup to allow wakeup in deep_sleep
    digitalWrite(TP_PWR_PIN, HIGH);                                             // Set his value to HIGH

    pinMode(LED_PIN, OUTPUT);                                                   // Configure the LED as an output

    pinMode(CHARGE_PIN, INPUT_PULLUP);                                          // Configure the charge pin as an input pullup
    attachInterrupt(CHARGE_PIN, [] {
        charge_indication = true;
    }, CHANGE);

    if (digitalRead(CHARGE_PIN) == LOW) {
        charge_indication = true;
    }
}


/****************************/
/********** SETUP ***********/
/****************************/

void setup(void){
    Serial.begin(115200);                                                       // Sets the communication rate in number of characters per second
    tft.init();                                                                 // Initialization of the screen
    tft.setRotation(1);                                                         // Set the rotation of the screen (here landscape)
    tft.setSwapBytes(true);                                                     // Swap color byte order when rendering
    tft.pushImage(0, 0,  160, 80, ttgo);                                        // Display an image at the start of the bracelet

    Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN);                                       // Set I2C pin
    Wire.setClock(400000);                                                      // Set clock

    // Run all necessary setup functions
    setupRTC();                                                                 // RTC setup
    setupMPU9250();                                                             // MPU setup
    setupADC();                                                                 // ADC setup
    setupWiFi();                                                                // WiFi setup
    setupOTA();                                                                 // OTA setup
    screenSetup();                                                              // Screen setup

    WiFi.mode(WIFI_STA);                                                        // Set device as a Wi-Fi Station

    // Init ESP-NOW
    if (esp_now_init() != ESP_OK) {                                             // If the ESP-NOW is well initialized, the program continues
      Serial.println(F("Error initializing ESP-NOW"));                          // If not, an error message is displayed on the serial monitor
      return;
    }

    // Define callback functions
    esp_now_register_send_cb(OnDataSent);                                       // Function for the sending of data
    esp_now_register_recv_cb(OnDataRecv);                                       // Function for the reception of data

    // Register peer
    esp_now_peer_info_t peerInfo;                                               // Associate peer info ro a variable
    memcpy(peerInfo.peer_addr, broadcastAddress, 6);                            // Copy the value of the broadcast address to the memory
    peerInfo.channel = 0;                                                       // Set channel to 0
    peerInfo.encrypt = false;                                                   // Set encryption to false

    // Display a message regarding of the success of the peer register
    if (esp_now_add_peer(&peerInfo) == ESP_OK) {                                // Test if the pairing is a success
      Serial.println(F("Connection established"));                              // Information of the success is printed
    }
    else {
      Serial.println(F("Failed to add peer"));                                  // If the pairing failed, the message is printed on the serial monitor
    }
}

// Function that return the value of the voltage
String getVoltage(){
    uint16_t v = analogRead(BATT_ADC_PIN);                                      // The value of the batterie is read by the ADC
    float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);  // The voltage is calculate via the previous value
    return String(battery_voltage) + "V";                                       // A string is return with the desired value
}

// Function that is used for the display of time, date and batterie level
void RTC_Show(){
    // Initialization of time if it is not already done
    if (targetTime < millis()) {                                                // Check if the targetTime value is lower than the current time
        RTC_Date datetime = rtc.getDateTime();                                  // If it is, we define the date with 3 variables (hours, minutes and seconds)
        hh = datetime.hour;
        mm = datetime.minute;
        ss = datetime.second;
        targetTime = millis() + 1000;                                           // Then we actualize the targetTime ad increase it of 1 second
        if (ss == 0 || initial) {                                               // Check if the second are equals to 0 or if the initial variale is true
            initial = 0;                                                        // Set initial value to 0 to end the if
            tft.setTextColor(TFT_GREEN, TFT_DARKGREY);                          // Set the color of the text and the background color
            tft.setCursor (8, 60);                                              // Set the cursor to establish the position of the text on the display
            tft.print(__DATE__);                                                // This use the standard ADAFruit small font
        }

        tft.setTextColor(TFT_BLUE, TFT_DARKGREY);                               // Set the color of the text and the background color
        tft.drawCentreString(getVoltage(), 120, 60, 1);                         // Next size up font 2


        // Update digital time
        uint8_t xpos = 6;                                                       // Set position on x axis
        uint8_t ypos = 0;                                                       // Set position on y axis
        if (omm != mm) {                                                        // Only redraw every minute to minimise flicker
            tft.setTextColor(TFT_DARKGREY, TFT_DARKGREY);                       // Set font and background colour
            tft.drawString("88:88", xpos, ypos, 7);                             // Overwrite the text to clear it
            tft.setTextColor(0xFBE0, TFT_DARKGREY);                             // Set font and background colour
            omm = mm;                                                           // omm takes the value of mm to avoid going back in this loop next time

            // When hours are lower than 10, we add a 0 before the first number
            if (hh < 10) xpos += tft.drawChar('0', xpos, ypos, 7);
            xpos += tft.drawNumber(hh, xpos, ypos, 7);
            xcolon = xpos;
            xpos += tft.drawChar(':', xpos, ypos, 7);

            // Same things with minutes
            if (mm < 10) xpos += tft.drawChar('0', xpos, ypos, 7);
            tft.drawNumber(mm, xpos, ypos, 7);
        }

        // Change the color of the ":" caracter depending of the second
        // Allow the user to have a visual effect for every second
        if (ss % 2) { // Flash the colon
            tft.setTextColor(0x39C4, TFT_DARKGREY);
            xpos += tft.drawChar(':', xcolon, ypos, 7);
            tft.setTextColor(0xFBE0, TFT_DARKGREY);
        } else {
            tft.drawChar(':', xcolon, ypos, 7);
        }
    }
}

// Function that program the wake-up conditions before enter the deep_sleep mode
void sleep(){
  IMU.setSleepEnabled(true);                                                    // Enable the IMU to sleep
  tft.writecommand(ST7735_DISPOFF);                                             // Turn off the display
  //esp_sleep_enable_timer_wakeup(60 * 1000000);                                // Set up a timer to wake the esp every minute to check if data were received
  esp_sleep_enable_ext1_wakeup(GPIO_SEL_33, ESP_EXT1_WAKEUP_ANY_HIGH);          // Allow to wake up if the user press the touch button
  esp_deep_sleep_start();                                                       // Enter deep_sleep mode
}


/****************************/
/*********** LOOP ***********/
/****************************/

void loop(){
#ifdef ARDUINO_OTA_UPDATE
    ArduinoOTA.handle();
#endif

    //! If OTA starts, skip the following operation
    if (otaStart)
        return;

    // Display of the charge indication
    if (charge_indication) {
        charge_indication = false;
        if (digitalRead(CHARGE_PIN) == LOW) {
            tft.pushImage(140, 55, 16, 16, charge);                             // If the bracelet is charging, we can display an image on the screen
        } else {
            tft.fillRect(140, 55, 16, 16, TFT_DARKGREY);                        // Fill screen with the color of the background if it is not charging
        }
    }

    // That part tell the program what to do when the touch button is pressed
    if (digitalRead(TP_PIN_PIN) == HIGH) {
        if (!pressed) {                                                         // When it is pressed only once time
            initial = 1;                                                        // Set the initial variable to 1
            targetTime = millis() + 1000;                                       // Update and add 1 second to targetTime
            tft.fillScreen(TFT_DARKGREY);                                       // Fill screen with background color
            omm = 99;                                                           // Set omm value to 99
            func_select = func_select + 1 > 2 ? 0 : func_select + 1;            // Increment the func_select if it is still on the range, or set it to 0
            digitalWrite(LED_PIN, HIGH);                                        // Turn on the LED
            delay(100);                                                         // Add a delay of 100ms
            digitalWrite(LED_PIN, LOW);                                         // Turn off the LED
            pressed = true;                                                     // Set pressed to True
            pressedTime = millis();                                             // Set pressedTime to current time
        } else {
            if(millis() - pressedTime > 3000){                                  // If the user press the button during 3 seconds
                tft.fillScreen(TFT_DARKGREY);                                   // The screen becomes unicolor
                delay(2000);                                                    // Wait 2 seconds
                sleep();                                                        // Enter in deep_sleep mode
              }
        }
    } else {
        pressed = false;                                                        // Set pressed as false if the user didn't pressed the button
    }

    currentTime = millis();                                                     // Set currentTime as the current time
    if(currentTime - pressedTime > 40000){                                      // Compare it to the last time the user pressed the button
      sleep();                                                                  // Enter in deep_sleep mode if the button is not touched during 40 seconds
    }

    // Switch loop that allow the user to naviguate between the 3 modes of the bracelet depending on the func_select value
    switch (func_select) {
      // First mode is the time and date display
      case 0:
          RTC_Show();                                                           // Run the RTC_Show function to display on the sreen all informations needed
          prise_spo2 = 0;                                                       // Set prise_spo2 value to 0
          break;
      // Second mode is the Heartbeat measure
      case 1:
          snprintf(buff, sizeof(buff), "BPM");                                  // Print on the screen the BPM
          tft.drawString(buff, 72, 64);                                         // Set the place were it is displayed
          value_bpm = BPM();                                                    // Run the BPM function
          if (value_bpm >= 40) {                                                // Check the quality of the value (we want only value higher than 40)
              moy += value_bpm;                                                 // Add values to a variable
              j++;                                                              // Increment a counter
              if(j == 4){                                                       // When we have 4 values
                moy /= 4;                                                       // Divide by 4 to obtain a average
                tft.setTextColor(0xFBE0, TFT_DARKGREY);                         // Set color of text and background
                tft.drawNumber(moy, tft.width() / 3.5, tft.height() / 10, 7);   // Display the result on the screen
                j = 0;                                                          // Set counter to 0
                moy = 0;                                                        // Set average to 0
              }
            }
          break;
      // Third mode is the Oxygenation rate measure
      case 2:
          snprintf(buff, sizeof(buff), "SPO2");                                 // Print on the screen the SPO2
          tft.drawString(buff, 70, 64);                                         // Set the place were it is displayed
          if(prise_spo2 == 0){                                                  // Check if the prise_spo2 value is equal to 0 to assure only one mesure
            vspo2 = mesureSpo2();                                               // Run the mesureSpo2 function
            prise_spo2++;;                                                      // Increment the prise_spo2 value
            if(vspo2 <= 100 && vspo2 > 0){                                      // Check the quality of the value (between 0 and 100%)
              tft.setTextColor(0xFBE0, TFT_DARKGREY);                           // Set color of text and background
              tft.drawNumber(vspo2, tft.width() / 3.5, tft.height() / 10, 7);   // Display the result on the screen
            }
          }
          break;
    }
}

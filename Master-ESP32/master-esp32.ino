/*
* The purpose of this code is to allow a master ESP32 to control the measurement
* (heartbeat and blood oxygenation rate) and retrieve these values 
* via the ESP-NOW protocol. This is the master code.
*/


/****************************/
/******** LIBRARIES *********/
/****************************/

#include <esp_now.h>
#include <WiFi.h>


/****************************/
/* VARIABLES INITIALISATION */
/****************************/

const char nom[10]="Master";                                                        // Name of the ESP to allow a better visualization
uint8_t broadcastAddress[] = {0x30, 0x83, 0x98, 0xDE, 0x0A, 0x34};                  // RECEIVER MAC ADDRESS

// Button
const int buttonPin = 22;
int buttonState = 0;
int envoi_suceed;

// Structure use in the ESP-NOW protocol, (must match the receiver structure)
typedef struct struct_message {
 char a[32];                                                                        // Name of the transmitter
 int bpm;                                                                           // BPM value
 int spo2;                                                                          // SPO2 value
} struct_message;
struct_message myData;
struct_message dataRcv;


/****************************/
/******** FUNCTIONS  ********/
/****************************/

// Callbacks for sending and receiving data
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
 Serial.print(F("\r\nMaster packet sent:\t"));
 Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail"); // Print the state of the delivery
 if(status == ESP_NOW_SEND_SUCCESS) {                                               // If the message is send
  envoi_suceed = 1;                                                                 // This variable get the value 1
 }
}

void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
 memcpy(&dataRcv, incomingData, sizeof(dataRcv));
 Serial.print("From : ");
 Serial.println(dataRcv.a);                                                         // Print the name of the transmitter of the message
 Serial.print("BPM :");
 Serial.println(dataRcv.bpm);                                                       // If actions are both -1, the slave send back measurements of BPM and SPO2
 Serial.print("SPO2 :");
 Serial.println(dataRcv.spo2);
 Serial.println();
}


/****************************/
/********** SETUP ***********/
/****************************/

void setup() {

 Serial.begin(115200); // Init Serial Monitor
 
 pinMode(buttonPin, INPUT_PULLUP); // Init button
 
 WiFi.mode(WIFI_STA); // Set device as a Wi-Fi Station

 // Init ESP-NOW
 if (esp_now_init() != ESP_OK) {
   Serial.println(F("Error initializing ESP-NOW"));
   return;
 }
 Serial.print(F("Reciever initilized : "));
 Serial.println(WiFi.macAddress());
 
 // Define callback functions
 esp_now_register_send_cb(OnDataSent);
 esp_now_register_recv_cb(OnDataRecv);
 
 // Register peer
 esp_now_peer_info_t peerInfo;
 peerInfo.channel = 0;
 peerInfo.encrypt = false;
   
 memcpy(peerInfo.peer_addr, broadcastAddress, 6);
 if (esp_now_add_peer(&peerInfo) != ESP_OK){
   Serial.println("Failed to add peer");
   return;
 }
}


/****************************/
/*********** LOOP ***********/
/****************************/

void loop() {

    buttonState = digitalRead(buttonPin);                                           // Read the state of the button

    if (buttonState == LOW) {                                                       // If it is pressed
      delay(1000);                                                                  // Wait 1 second
      envoi_suceed = 0;                                                             // Set envoi_suceed to 0
      
      strcpy(myData.a, nom);                                                        // Set values to send
      myData.bpm = -1;                                                              // bpm takes 0
      myData.spo2 = -1;                                                             // spo2 takes 0 too
      esp_err_t result0 = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData)); // Check the sending
      delay(1000);                                                                  // Wait 1 second
      while(envoi_suceed == 0) {                                                    // While the message is not received
        delay(1000);                                                                // Wait 1 second
        esp_err_t result0 = esp_now_send(broadcastAddress, (uint8_t *) &myData, sizeof(myData)); // Try to send it again
        delay(1000);                                                                // Wait 1 second
      }
    }
}

/** NimBLE_Server Demo:

    Demonstrates many of the available features of the NimBLE client library.

    Created: on March 24 2020
        Author: H2zero

*/

#include <NimBLEDevice.h>

void scanEndedCB(NimBLEScanResults results);

static NimBLEAdvertisedDevice* advDevice;
static bool doConnect = false;
static uint32_t scanTime = 5; /** 0 = scan forever */

std::vector<NimBLERemoteService*>* victor;
std::vector<NimBLERemoteCharacteristic*>* gaetan;
std::vector<NimBLERemoteDescriptor*>* clement;

/**  None of these are required as they will be handled by the library with defaults. **
 **                       Remove as you see fit for your needs                        */
class ClientCallbacks : public NimBLEClientCallbacks {
    void onConnect(NimBLEClient* pClient) {
      victor = pClient->getServices(true);
      for (int i = 0; i < (*victor).size(); i++) {
        Serial.println((*victor).at(i)->getUUID().toString().c_str());
      }
      Serial.println((*victor).size());
      Serial.println("Connected");
      /** After connection we should change the parameters if we don't need fast response times.
          These settings are 150ms interval, 0 latency, 450ms timout.
          Timeout should be a multiple of the interval, minimum is 100ms.
          I find a multiple of 3-5 * the interval works best for quick response/reconnect.g
          Min interval: 120 * 1.25ms = 150, Max interval: 120 * 1.25ms = 150, 0 latency, 60 * 10ms = 600ms timeout
      */
      pClient->updateConnParams(120, 120, 0, 60);
    };

    void onDisconnect(NimBLEClient* pClient) {
      Serial.print(pClient->getPeerAddress().toString().c_str());
      Serial.println(" Disconnected - Starting scan");
      NimBLEDevice::getScan()->start(scanTime, scanEndedCB);
    };

    /** Called when the peripheral requests a change to the connection parameters.
        Return true to accept and apply them or false to reject and keep
        the currently used parameters. Default will return true.
    */
    bool onConnParamsUpdateRequest(NimBLEClient* pClient, const ble_gap_upd_params* params) {
      if (params->itvl_min < 24) { /** 1.25ms units */
        return false;
      } else if (params->itvl_max > 40) { /** 1.25ms units */
        return false;
      } else if (params->latency > 2) { /** Number of intervals allowed to skip */
        return false;
      } else if (params->supervision_timeout > 100) { /** 10ms units */
        return false;
      }

      return true;
    };

    /********************* Security handled here **********************
    ****** Note: these are the same return values as defaults ********/
    uint32_t onPassKeyRequest() {
      Serial.println("Client Passkey Request");
      /** return the passkey to send to the server */
      return 123456;
    };

    bool onConfirmPIN(uint32_t pass_key) {
      Serial.print("The passkey YES/NO number: ");
      Serial.println(pass_key);
      /** Return false if passkeys don't match. */
      return true;
    };

    /** Pairing process complete, we can check the results in ble_gap_conn_desc */
    void onAuthenticationComplete(ble_gap_conn_desc* desc) {
      if (!desc->sec_state.encrypted) {
        Serial.println("Encrypt connection failed - disconnecting");
        /** Find the client with the connection handle provided in desc */
        NimBLEDevice::getClientByID(desc->conn_handle)->disconnect();
        return;
      }
    };
};


/** Define a class to handle the callbacks when advertisments are received */
class AdvertisedDeviceCallbacks: public NimBLEAdvertisedDeviceCallbacks {

    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
      Serial.print("Advertised Device found: ");
      Serial.println(advertisedDevice->toString().c_str());

      if (advertisedDevice->isAdvertisingService(NimBLEUUID("180a")))
      {
        Serial.println("Found Our Service");
        /** stop scan before connecting */
        NimBLEDevice::getScan()->stop();
        NimBLEDevice::setMTU(185);
        Serial.println(NimBLEDevice::getMTU());

        /** Save the device reference in a global for the client to use*/
        advDevice = advertisedDevice;
        /** Ready to connect now */
        doConnect = true;
      }
    };
};


/** Notification / Indication receiving handler callback */
void notifyCB(NimBLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  Serial.println("");
  std::string str = (isNotify == true) ? "Notif" : "Indication";
  str += " from ";

  str += std::string(pRemoteCharacteristic->getRemoteService()->getClient()->getPeerAddress());
  str += ": Service = " + std::string(pRemoteCharacteristic->getRemoteService()->getUUID());
  str += ", Characteristic = " + std::string(pRemoteCharacteristic->getUUID());
  str += ", Value = " + std::string((char*)pData, length);
  Serial.println(str.c_str());

  Serial.print("pData: ");
  for (int i = 0; i < length; i++) {
    Serial.print(*(pData + i));
    Serial.print("-");
  }
  Serial.println("");

  if (pRemoteCharacteristic->getUUID() == NimBLEUUID("f000efe1-0451-4000-0000-00000000b000")) {
    Serial.print("HEART RATE : ");
    Serial.print(*(pData + 12));
    Serial.println(" BPM");
    Serial.println("");
  }
}



/** Callback to process the results of the last scan or restart it */
void scanEndedCB(NimBLEScanResults results) {
  Serial.println("Scan Ended");
}


/** Create a single global instance of the callback class to be used by all clients */
static ClientCallbacks clientCB;


/** Handles the provisioning of clients and connects / interfaces with the server */
bool connectToServer() {
  NimBLEClient* pClient = nullptr;

  /** Check if we have a client we should reuse first **/
  if (NimBLEDevice::getClientListSize()) {
    /** Special case when we already know this device, we send false as the
        second argument in connect() to prevent refreshing the service database.
        This saves considerable time and power.
    */
    pClient = NimBLEDevice::getClientByPeerAddress(advDevice->getAddress());
    if (pClient) {
      if (!pClient->connect(advDevice, false)) {
        Serial.println("Reconnect failed");
        return false;
      }
      Serial.println("Reconnected client");
    }
    /** We don't already have a client that knows this device,
        we will check for a client that is disconnected that we can use.
    */
    else {
      pClient = NimBLEDevice::getDisconnectedClient();
    }
  }

  /** No client to reuse? Create a new one. */
  if (!pClient) {
    if (NimBLEDevice::getClientListSize() >= NIMBLE_MAX_CONNECTIONS) {
      Serial.println("Max clients reached - no more connections available");
      return false;
    }

    pClient = NimBLEDevice::createClient();

    Serial.println("New client created");

    pClient->setClientCallbacks(&clientCB, false);
    /** Set initial connection parameters: These settings are 15ms interval, 0 latency, 120ms timout.
        These settings are safe for 3 clients to connect reliably, can go faster if you have less
        connections. Timeout should be a multiple of the interval, minimum is 100ms.
        Min interval: 12 * 1.25ms = 15, Max interval: 12 * 1.25ms = 15, 0 latency, 51 * 10ms = 510ms timeout
    */
    pClient->setConnectionParams(12, 12, 0, 51);
    /** Set how long we are willing to wait for the connection to complete (seconds), default is 30. */
    pClient->setConnectTimeout(5);


    if (!pClient->connect(advDevice)) {
      /** Created a client but failed to connect, don't need to keep it as it has no data */
      NimBLEDevice::deleteClient(pClient);
      Serial.println("Failed to connect, deleted client");
      return false;
    }
  }

  if (!pClient->isConnected()) {
    if (!pClient->connect(advDevice)) {
      Serial.println("Failed to connect");
      return false;
    }
  }

  Serial.print("Connected to: ");
  Serial.println(pClient->getPeerAddress().toString().c_str());


  /** Now we can read/write/subscribe the charateristics of the services we are interested in */
  NimBLERemoteService* pSvc = nullptr;
  NimBLERemoteCharacteristic* pChr = nullptr;
  NimBLERemoteDescriptor* pDsc = nullptr;


  for (int i = 0; i < (*victor).size(); i++) {
    pSvc = (*victor).at(i);
    Serial.print("\n\nSERVICE ");
    Serial.println((*victor).at(i)->getUUID().toString().c_str());


    if (pSvc) {    /** make sure it's not null */
      gaetan = pSvc->getCharacteristics(true);
      for (int j = 0; j < (*gaetan).size(); j++) {
        Serial.print("    ");
        Serial.print("CHARACTERISTIC ");
        Serial.println((*gaetan).at(j)->getUUID().toString().c_str());
        pChr = (*gaetan).at(j);


        if (pChr) {

          if (pChr->canRead()) {
            Serial.println("        CAN READ ");
            Serial.print("          Characteristic Value: ");
            Serial.println(pChr->readValue().c_str());
          }

          if (pChr->canWrite()) {
            Serial.println("        CAN WRITE ");
            //const uint8_t notificationOn[] = {0xfc, 0x09, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf9}; //heart rate
            //const uint8_t notificationOn[] = {0xfc, 0x10, 0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf3}; //alert
            //const uint8_t notificationOn[] = {0xfc, 0x0f, 0x04, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xef}; //luminosité (0x00/0x01/0x02/) trouvé dans bleCmd
            //const uint8_t notificationOn[] = {0xfc, 0x01, 0x00, 0x01, 0x02, 0x02, 0x00, 0x00, 0x13, 0x46, 0x08, 0x30, 0x09, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x64}; //réveil à 13h46
            //const uint8_t notificationOn[] = {0xfc, 0x35, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xcd}; //changer le cadan (2e cadran)
            //const uint8_t notificationOn[] = {0xfc, 0x0f, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf4}; //restart




            const uint8_t notificationOn2[] = {0xfc, 0x09, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xf5};
            if (pChr->getUUID() == NimBLEUUID("f000efe1-0451-4000-0000-00000000b000")) {
              if (pChr->writeValue((uint8_t*)notificationOn2, 20, true)) {
                Serial.print("Wrote new value to: ");
                Serial.println(pChr->getUUID().toString().c_str());
              }
            }


            /*
              if(pChr->getUUID() == NimBLEUUID("f000efe1-0451-4000-0000-00000000b000")){
              for (int i = 0; i<10; i++){
               for (int j = 9; j<18; j++){
                 for (int k = 0; k<256; k++){
                   if ((j==9 & i==2)||(j==15)){
                     Serial.println("BONSOIR NON!!!!");
                   }
                   else{
                     uint8_t notificationOn[] = {0xfc, (byte)j, (byte)i, (byte)k, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, (byte)((~(252+i+j+k)+1)&0xff)};
                     Serial.print("fc ");
                     Serial.print((byte)j);
                     Serial.print(" ");
                     Serial.print((byte)i);
                     Serial.print(" ");
                     Serial.print((byte)k);
                     Serial.print("    ");
                     Serial.print("crc : ");
                     Serial.println((byte)((~(252+i+j+k)+1)&0xff));
                     if(pChr->writeValue((uint8_t*)notificationOn, 20, true)) {
                         Serial.print("Wrote new value to: ");
                         Serial.println(pChr->getUUID().toString().c_str());
                     }
                   }
                 }
               }
              }*
            */

          }
        






        if (pChr->canNotify()) {
          Serial.println("       CAN NOTIFY ");
          if (pChr->subscribe(true, notifyCB, true)) {
            Serial.println("       SUBSCRIBED NOTIFY");
          }
          if (pChr->registerForNotify(notifyCB, true, true)) {
            Serial.println("       REGISTERED FOR NOTIFY");
          }
        }
        else if (pChr->canIndicate()) {
          Serial.println("       CAN INDICATE ");
          if (pChr->subscribe(false, notifyCB)) {
            Serial.println("       SUBSCRIBED INDICATE");
          }
        }

        clement = pChr->getDescriptors(true);
        for (int k = 0; k < (*clement).size(); k++) {
          const uint8_t notificationOn2[] = {0x1, 0x0};
          Serial.print("      ");
          Serial.print("DESCRIPTOR ");
          Serial.println((*clement).at(k)->getUUID().toString().c_str());
          pDsc = (*clement).at(k);
          pDsc->writeValue((uint8_t*)notificationOn2, 2, false);
          Serial.print("          Descriptor Value: ");
          Serial.println(pDsc->readValue().c_str());
          Serial.println(pDsc->readUInt8());
        }
        }
      }


    } else {
      Serial.println("Service not found.");
    }

  }
  Serial.println("Done with this device!");
  return true;
}

void setup () {
  Serial.begin(115200);
  Serial.println("Starting NimBLE Client");
  /** Initialize NimBLE, no device name spcified as we are not advertising */
  NimBLEDevice::init("");

  /** Set the IO capabilities of the device, each option will trigger a different pairing method.
      BLE_HS_IO_KEYBOARD_ONLY    - Passkey pairing
      BLE_HS_IO_DISPLAY_YESNO   - Numeric comparison pairing
      BLE_HS_IO_NO_INPUT_OUTPUT - DEFAULT setting - just works pairing
  */
  //NimBLEDevice::setSecurityIOCap(BLE_HS_IO_KEYBOARD_ONLY); // use passkey
  //NimBLEDevice::setSecurityIOCap(BLE_HS_IO_DISPLAY_YESNO); //use numeric comparison

  /** 2 different ways to set security - both calls achieve the same result.
      no bonding, no man in the middle protection, secure connections.

      These are the default values, only shown here for demonstration.
  */
  //NimBLEDevice::setSecurityAuth(false, false, true);
  NimBLEDevice::setSecurityAuth(/*BLE_SM_PAIR_AUTHREQ_BOND | BLE_SM_PAIR_AUTHREQ_MITM |*/ BLE_SM_PAIR_AUTHREQ_SC);

  /** Optional: set the transmit power, default is 3db */
  NimBLEDevice::setPower(ESP_PWR_LVL_P9); /** +9db */

  /** Optional: set any devices you don't want to get advertisments from */
  // NimBLEDevice::addIgnored(NimBLEAddress ("aa:bb:cc:dd:ee:ff"));

  /** create new scan */
  NimBLEScan* pScan = NimBLEDevice::getScan();

  /** create a callback that gets called when advertisers are found */
  pScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());

  /** Set scan interval (how often) and window (how long) in milliseconds */
  pScan->setInterval(45);
  pScan->setWindow(15);

  /** Active scan will gather scan response data from advertisers
      but will use more energy from both devices
  */
  pScan->setActiveScan(true);
  /** Start scanning for advertisers for the scan time specified (in seconds) 0 = forever
      Optional callback for when scanning stops.
  */
  pScan->start(scanTime, scanEndedCB);
}


void loop () {
  /** Loop here until we find a device we want to connect to */
  while (!doConnect) {
    delay(1);
  }

  doConnect = false;

  /** Found a device we want to connect to, do it now */
  if (connectToServer()) {
    Serial.println("Success! we should now be getting notifications, scanning for more!");
  } else {
    Serial.println("Failed to connect, starting scan");
  }

  NimBLEDevice::getScan()->start(scanTime, scanEndedCB);
}

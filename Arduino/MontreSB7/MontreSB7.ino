/** tiré de https://electropeak.com/learn/esp32-bluetooth-low-energy-ble-on-arduino-ide-tutorial/
 *  
 * A BLE client example that is rich in capabilities.
 * There is a lot new capabilities implemented.
 * author unknown
 * updated by chegewara
 */

 // pb lire valeur descrpteur https://github.com/nkolban/esp32-snippets/issues/423

#include "BLEDevice.h"
const uint8_t bothOff[]        = {0x0, 0x0};
const uint8_t notificationOn[] = {0x1, 0x0};
const uint8_t indicationOn[]   = {0x2, 0x0};
const uint8_t bothOn[]         = {0x3, 0x0};
const bool Ok_Print_2D00 = true;
const bool Ok_Print_2D01 = true;
const bool Ok_Print = true;
const bool Ok_Print_Decouverte = true;
const bool Ok_Print_Scan = true;

static BLEUUID Serv1800("00001800-0000-1000-8000-00805f9b34fb");
static BLERemoteService* Service1800;
static BLEUUID Char2A00("00002a00-0000-1000-8000-00805f9b34fb");
static BLERemoteCharacteristic* Characteristic2A00;
static BLEUUID Char2A01("00002a01-0000-1000-8000-00805f9b34fb");
static BLERemoteCharacteristic* Characteristic2A01;
static BLEUUID Char2A04("00002a04-0000-1000-8000-00805f9b34fb");
static BLERemoteCharacteristic* Characteristic2A04;

static BLEUUID Serv1801("00001801-0000-1000-8000-00805f9b34fb");
static BLERemoteService* Service1801;
static BLEUUID Char2A05("00002a05-0000-1000-8000-00805f9b34fb");
static BLERemoteCharacteristic* Characteristic2A05;
bool Donnee_prete_2A05 = false;

static BLEUUID Serv180A("0000180a-0000-1000-8000-00805f9b34fb");
static BLERemoteService* Service180A;
static BLEUUID Char2A23("00002a23-0000-1000-8000-00805f9b34fb");
static BLERemoteCharacteristic* Characteristic2A23;
static BLEUUID Char2A24("00002a24-0000-1000-8000-00805f9b34fb");
static BLERemoteCharacteristic* Characteristic2A24;
static BLEUUID Char2A25("00002a25-0000-1000-8000-00805f9b34fb");
static BLERemoteCharacteristic* Characteristic2A25;
static BLEUUID Char2A26("00002a26-0000-1000-8000-00805f9b34fb");
static BLERemoteCharacteristic* Characteristic2A26;
static BLEUUID Char2A27("00002a27-0000-1000-8000-00805f9b34fb");
static BLERemoteCharacteristic* Characteristic2A27;
static BLEUUID Char2A28("00002a28-0000-1000-8000-00805f9b34fb");
static BLERemoteCharacteristic* Characteristic2A28;
static BLEUUID Char2A29("00002a29-0000-1000-8000-00805f9b34fb");
static BLERemoteCharacteristic* Characteristic2A29;
static BLEUUID Char2A2A("00002a2a-0000-1000-8000-00805f9b34fb");
static BLERemoteCharacteristic* Characteristic2A2A;
static BLEUUID Char2A50("00002a50-0000-1000-8000-00805f9b34fb");
static BLERemoteCharacteristic* Characteristic2A50;

static BLEUUID Serv18D0("000018d0-0000-1000-8000-00805f9b34fb");
static BLERemoteService* Service18D0;
static BLEUUID Char2D00("00002d00-0000-1000-8000-00805f9b34fb");
static BLERemoteCharacteristic* Characteristic2D00;
bool Donnee_prete_2D00 = false;
static BLEUUID Char2D01("00002d01-0000-1000-8000-00805f9b34fb");
static BLERemoteCharacteristic* Characteristic2D01;

static BLEUUID ServFF01("5833ff01-9b8b-5191-6142-22a4536ef123");
static BLERemoteService* ServiceFF01;
static BLEUUID CharFF02("5833ff02-9b8b-5191-6142-22a4536ef123");
static BLERemoteCharacteristic* CharacteristicFF02;
static BLEUUID CharFF03("5833ff03-9b8b-5191-6142-22a4536ef123");
static BLERemoteCharacteristic* CharacteristicFF03;
bool Donnee_prete_FF03 = false;

BLEUUID DescriptorUUID;
static BLERemoteDescriptor* Descriptor2A05_2902;
static BLERemoteDescriptor* Descriptor2D01_2901;
static BLERemoteDescriptor* Descriptor2D00_2901;
static BLERemoteDescriptor* Descriptor2D00_2902;
static BLERemoteDescriptor* DescriptorFF03_2902;

String My_BLE_Address = "c0:00:c5:d5:22:ce"; //Hardware Bluetooth MAC of my fitnessband, will vary for every band obtained through nRF connect application 
//String My_BLE_Address = "08:3a:f2:a9:79:26"; //Hardware Bluetooth MAC of my simul sb7+ esp32

static BLEAddress *Server_BLE_Address;
String Scaned_BLE_Address;
static BLEAddress *Server_BLE_Address_ok;
String Scaned_BLE_Address_ok;

static boolean doConnect = false;
static boolean connected = false;
static boolean doScan = false;
static BLEAdvertisedDevice* myDevice;
static BLEClient*  pClient;

BLEUUID ServiceUUID;
BLEUUID CharacteristicUUID;

static void notifyCallback2A05(
  BLERemoteCharacteristic* pBLERemoteCharacteristicRead,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.println("                 On passe dans notifyCallback 2A05 !!!!!!!!!!!!!!!!!!!!!!!!!");
    Donnee_prete_2A05 = true;
}

static void notifyCallback2D00(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
//    if (Ok_Print_2D01) Serial.print("      NC_2D00 = ");
    Donnee_prete_2D00 = true;
/*    Serial.print("    Notify callback for characteristic :  ");
    Serial.print(pBLERemoteCharacteristic->getUUID().toString().c_str());
    Serial.print("    ");*/
/*    for (int i = 0; i < length; i++)
    {
      Serial.printf("%02X",  pData[i]);

//       Serial.print(pData[i],HEX);
       Serial.print("-");
    }
    Serial.println(" ");*/
//    Serial.print(pBLERemoteCharacteristic->readValue().c_str());
/*    Serial.print("    ");
    Serial.print(length);
    Serial.print("  ");
    Serial.println(isNotify);*/
}

static void notifyCallbackFF03(
  BLERemoteCharacteristic* pBLERemoteCharacteristicRead,
  uint8_t* pData,
  size_t length,
  bool isNotify) {
    Serial.println("                 On passe dans notifyCallback FF03  !!!!!!!!!!!!!!!!!!!!!!!!!");
    Donnee_prete_FF03 = true;
}

void Ecrire_2D01(std::string newvalue, bool ok_print)
{
  if (ok_print)
  {
    for (int i=0; i<newvalue.length(); i++)
    {
      Serial.printf("%02X",  byte(newvalue[i]));
      if (i == newvalue.length()-1)
      Serial.println(" ");
      else Serial.print("-");
    }
  }  
  uint8_t Data[byte(newvalue[1])];
  for (int i=0; i<newvalue.length(); i++)
  {
    Data[i] = byte(newvalue[i]);
  }
  Characteristic2D01->writeValue(Data, newvalue.length());
}

String Lecture_2D00(bool ok_print)
{
String str;
std::string newvalue;
  std::string value = Characteristic2D00->readValue();
  str = "";
  newvalue = "";
  for (int i=0; i < value[1]; i++)
  {
    str = str + char(value[i]);
    newvalue = newvalue + value[i];
  }
  char chk = CheckSum(newvalue);
  if ((newvalue[0] == char(0xAC)) | (newvalue[0] == char(0x5A)))
  {
     // La chaine est OK
  }
  if (ok_print)
  {
    if (chk == newvalue[byte(newvalue[1])])
      Serial.println("    Checksum valide");
      else Serial.println("    Checksum invalide");
    Serial.print("    The string was: ");
    Serial.print("                 ");
    for (int i = 0; i < str.length(); i++)
    {
      Serial.printf("%02X",  byte(str[i]));
      if (i < str.length()-1)
        Serial.print("-");
        else Serial.println("");
    }
  }
  return str;
}

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    connected = true;
    Serial.println("onConnect");
  }
  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println("onDisconnect");
  }
};

bool connectToServer() {
  Serial.print("Forming a connection to ");
  Serial.println(myDevice->getAddress().toString().c_str());
  pClient  = BLEDevice::createClient();
  Serial.println(" - Created client");
  pClient->setClientCallbacks(new MyClientCallback());
  // Connect to the remove BLE Server.
  pClient->connect(myDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
  Serial.println(" - Connected to server");
  Serial.println(" ");
  
  // Obtain a reference to the service we are after in the remote BLE server.
  std::map<std::string, BLERemoteService*>* servicesMap = pClient->getServices();
  if (Ok_Print_Decouverte) Serial.println("Services : " + String(servicesMap->size()));  
  for (auto &myServiceEntry : *servicesMap)   
  {
    BLERemoteService* svc = myServiceEntry.second;
    String sservice = svc->getUUID().toString().c_str();
    if (Ok_Print_Decouverte) Serial.printf("Service found on Device: %s \n",  svc->getUUID().toString().c_str());
    if (sservice == Serv1800.toString().c_str())
    {
      if (Ok_Print_Decouverte) Serial.println (" 1800                                                                                     ok repéré!");
      Service1800 = pClient->getService(Serv1800);
      std::map<std::string, BLERemoteCharacteristic*>* CharacteristicsMap = Service1800-> getCharacteristics();
      if (Ok_Print_Decouverte) Serial.println("   Characteristic : " + String(CharacteristicsMap->size()));  
      for (auto &myCharacteristicEntry : *CharacteristicsMap)   
      {
        BLERemoteCharacteristic* chr = myCharacteristicEntry.second;
        if (Ok_Print_Decouverte) Serial.printf("   Characteristic found on Device: %s \n",  chr->getUUID().toString().c_str());
        String scharacteristic = chr->getUUID().toString().c_str();
        if (scharacteristic == Char2A00.toString().c_str())
        {
          if (Ok_Print_Decouverte) Serial.println ("   2A00                                                                                     ok repéré!");
          Characteristic2A00 = Service1800->getCharacteristic(Char2A00);
          if (Characteristic2A00->canBroadcast())       scharacteristic = scharacteristic + "[B]";
          if (Characteristic2A00->canIndicate())        scharacteristic = scharacteristic + "[I]";
          if (Characteristic2A00->canNotify())          scharacteristic = scharacteristic + "[N]";
          if (Characteristic2A00->canRead())            scharacteristic = scharacteristic + "[R]";
          if (Characteristic2A00->canWrite())           scharacteristic = scharacteristic + "[W]";
          if (Characteristic2A00->canWriteNoResponse()) scharacteristic = scharacteristic + "[WnR]";
          if (Ok_Print_Decouverte) Serial.print("   "+scharacteristic);
          if (Characteristic2A00->canRead())
          {
            std::string value = Characteristic2A00->readValue();
            if (Ok_Print_Decouverte)
            { 
              Serial.print("           Longueur : ");
              Serial.print(value.length());
              Serial.print("   ");
              Serial.print(value.c_str());
              Serial.print("           The Characteristic value was:     ");
              for (int i = 0; i < value.length(); i++)
              {
                Serial.print(byte(value[i]),HEX);
                if (i < value.length()-1)
                  Serial.print("-");
                  else Serial.println("");
              }
            }
          }
        }
        if (scharacteristic == Char2A01.toString().c_str())
        {
          if (Ok_Print_Decouverte) Serial.println ("   2A01                                                                                     ok repéré!");
          Characteristic2A01 = Service1800->getCharacteristic(Char2A01);
          if (Characteristic2A01->canBroadcast())       scharacteristic = scharacteristic + "[B]";
          if (Characteristic2A01->canIndicate())        scharacteristic = scharacteristic + "[I]";
          if (Characteristic2A01->canNotify())          scharacteristic = scharacteristic + "[N]";
          if (Characteristic2A01->canRead())            scharacteristic = scharacteristic + "[R]";
          if (Characteristic2A01->canWrite())           scharacteristic = scharacteristic + "[W]";
          if (Characteristic2A01->canWriteNoResponse()) scharacteristic = scharacteristic + "[WnR]";
          if (Ok_Print_Decouverte) Serial.print("   "+scharacteristic);
          if (Characteristic2A01->canRead())
          {
            std::string value = Characteristic2A01->readValue();
            if (Ok_Print_Decouverte) 
            {
              Serial.print("           Longueur : ");
              Serial.print(value.length());
              Serial.print("   ");
              Serial.print(value.c_str());
              Serial.print("           The Characteristic value was:     ");
              for (int i = 0; i < value.length(); i++)
              {
                Serial.print(byte(value[i]),HEX);
                if (i < value.length()-1)
                  Serial.print("-");
                  else Serial.println("");
              }
            }
          }  
        }
        if (scharacteristic == Char2A04.toString().c_str())
        {
          if (Ok_Print_Decouverte) Serial.println ("   2A04                                                                                     ok repéré!");
          Characteristic2A04 = Service1800->getCharacteristic(Char2A04);
          if (Characteristic2A04->canBroadcast())       scharacteristic = scharacteristic + "[B]";
          if (Characteristic2A04->canIndicate())        scharacteristic = scharacteristic + "[I]";
          if (Characteristic2A04->canNotify())          scharacteristic = scharacteristic + "[N]";
          if (Characteristic2A04->canRead())            scharacteristic = scharacteristic + "[R]";
          if (Characteristic2A04->canWrite())           scharacteristic = scharacteristic + "[W]";
          if (Characteristic2A04->canWriteNoResponse()) scharacteristic = scharacteristic + "[WnR]";
          if (Ok_Print_Decouverte) Serial.print("   "+scharacteristic);
          if (Characteristic2A04->canRead())
          {
            std::string value = Characteristic2A04->readValue();
            if (Ok_Print_Decouverte) 
            {
              Serial.print("           Longueur : ");
              Serial.print(value.length());
              Serial.print("   ");
              Serial.print(value.c_str());
              Serial.print("           The Characteristic value was:     ");
              for (int i = 0; i < value.length(); i++)
              {
                Serial.print(byte(value[i]),HEX);
                if (i < value.length()-1)
                  Serial.print("-");
                  else Serial.println("");
              }
            }
          }  
        }
      }
    }
    if (sservice == Serv1801.toString().c_str())
    {
      if (Ok_Print_Decouverte) Serial.println (" 1801                                                                                     ok repéré!");
      Service1801 = pClient->getService(Serv1801);
      std::map<std::string, BLERemoteCharacteristic*>* CharacteristicsMap = Service1801-> getCharacteristics();
      if (Ok_Print_Decouverte) Serial.println("   Characteristic : " + String(CharacteristicsMap->size()));  
      for (auto &myCharacteristicEntry : *CharacteristicsMap)   
      {
        BLERemoteCharacteristic* chr = myCharacteristicEntry.second;
        if (Ok_Print_Decouverte) Serial.printf("   Characteristic found on Device: %s \n",  chr->getUUID().toString().c_str());
        String scharacteristic = chr->getUUID().toString().c_str();
        if (scharacteristic == Char2A05.toString().c_str())
        {
          if (Ok_Print_Decouverte) Serial.println ("   2A05                                                                                     ok repéré!");
          Characteristic2A05 = Service1801->getCharacteristic(Char2A05);
          if (Characteristic2A05->canBroadcast())       scharacteristic = scharacteristic + "[B]";
          if (Characteristic2A05->canIndicate())        scharacteristic = scharacteristic + "[I]";
          if (Characteristic2A05->canNotify())          scharacteristic = scharacteristic + "[N]";
          if (Characteristic2A05->canRead())            scharacteristic = scharacteristic + "[R]";
          if (Characteristic2A05->canWrite())           scharacteristic = scharacteristic + "[W]";
          if (Characteristic2A05->canWriteNoResponse()) scharacteristic = scharacteristic + "[WnR]";
          if (Ok_Print_Decouverte) Serial.println("   "+scharacteristic);
          std::map<std::string, BLERemoteDescriptor*>* DescriptorsMap = Characteristic2A05-> getDescriptors();
          if (Ok_Print_Decouverte) Serial.println("      Descriptor: " + String(DescriptorsMap->size()));
          for (auto &myDescriptorEntry : *DescriptorsMap)   
          {
            BLERemoteDescriptor* des = myDescriptorEntry.second;
            if (Ok_Print_Decouverte) Serial.printf("      Descriptor found on Device: %s \n",  des->getUUID().toString().c_str());
            DescriptorUUID = des->getUUID();
            String sdescriptor = des->getUUID().toString().c_str();
            if (sdescriptor == "00002902-0000-1000-8000-00805f9b34fb")
            {
              Descriptor2A05_2902 = Characteristic2A05->getDescriptor(DescriptorUUID);
              Characteristic2A05->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)indicationOn, 2, true);
              if(Characteristic2A05->canNotify())
                 Characteristic2A05->registerForNotify(notifyCallback2A05);
              std::string value = Descriptor2A05_2902->readValue();
              if (Ok_Print_Decouverte) 
              {
                Serial.print("           Longueur : ");
                Serial.print(value.length());
                Serial.print("           The Descriptor value was: ");
                String s = value.c_str();
                Serial.print(s);
                Serial.print("           The Descriptor value was: ");
                Serial.print(" ");
                for (int i = 0; i < value.length(); i++)
                {
                  Serial.print(byte(value[i]),HEX);
                  if (i < value.length()-1)
                  {
                    Serial.print("-");
                  }
                  else Serial.println("");
                }
                Serial.println(" ");
              }
            }  
          }
        }
      }
    }
    if (sservice == Serv180A.toString().c_str())
    {
      if (Ok_Print_Decouverte) Serial.println (" 180A                                                                                     ok repéré!");
      Service180A = pClient->getService(Serv180A);
      std::map<std::string, BLERemoteCharacteristic*>* CharacteristicsMap = Service180A-> getCharacteristics();
      if (Ok_Print_Decouverte) Serial.println("   Characteristic : " + String(CharacteristicsMap->size()));  
      for (auto &myCharacteristicEntry : *CharacteristicsMap)   
      {
        BLERemoteCharacteristic* chr = myCharacteristicEntry.second;
        if (Ok_Print_Decouverte) Serial.printf("   Characteristic found on Device: %s \n",  chr->getUUID().toString().c_str());
        String scharacteristic = chr->getUUID().toString().c_str();
        if (scharacteristic == Char2A23.toString().c_str())
        {
          if (Ok_Print_Decouverte) Serial.println ("   2A23                                                                                     ok repéré!");
          Characteristic2A23 = Service180A->getCharacteristic(Char2A23);
          if (Characteristic2A23->canBroadcast())       scharacteristic = scharacteristic + "[B]";
          if (Characteristic2A23->canIndicate())        scharacteristic = scharacteristic + "[I]";
          if (Characteristic2A23->canNotify())          scharacteristic = scharacteristic + "[N]";
          if (Characteristic2A23->canRead())            scharacteristic = scharacteristic + "[R]";
          if (Characteristic2A23->canWrite())           scharacteristic = scharacteristic + "[W]";
          if (Characteristic2A23->canWriteNoResponse()) scharacteristic = scharacteristic + "[WnR]";
          if (Ok_Print_Decouverte) Serial.print("   "+scharacteristic);
          if (Characteristic2A23->canRead())
          {
            std::string value = Characteristic2A23->readValue();
            if (Ok_Print_Decouverte) 
            {
              Serial.print("           Longueur : ");
              Serial.print(value.length());
              Serial.print("   ");
              Serial.print(value.c_str());
              Serial.print("           The Characteristic value was:     ");
              for (int i = 0; i < value.length(); i++)
              {
                Serial.print(byte(value[i]),HEX);
                if (i < value.length()-1)
                  Serial.print("-");
                  else Serial.println("");
              }
            }
          }  
        }
        if (scharacteristic == Char2A24.toString().c_str())
        {
          if (Ok_Print_Decouverte) Serial.println ("   2A24                                                                                     ok repéré!");
          Characteristic2A24 = Service180A->getCharacteristic(Char2A24);
          if (Characteristic2A24->canBroadcast())       scharacteristic = scharacteristic + "[B]";
          if (Characteristic2A24->canIndicate())        scharacteristic = scharacteristic + "[I]";
          if (Characteristic2A24->canNotify())          scharacteristic = scharacteristic + "[N]";
          if (Characteristic2A24->canRead())            scharacteristic = scharacteristic + "[R]";
          if (Characteristic2A24->canWrite())           scharacteristic = scharacteristic + "[W]";
          if (Characteristic2A24->canWriteNoResponse()) scharacteristic = scharacteristic + "[WnR]";
          if (Ok_Print_Decouverte) Serial.print("   "+scharacteristic);
          if (Characteristic2A24->canRead())
          {
            std::string value = Characteristic2A24->readValue();
            if (Ok_Print_Decouverte) 
            {
              Serial.print("           Longueur : ");
              Serial.print(value.length());
              Serial.print("   ");
              Serial.print(value.c_str());
              Serial.print("           The Characteristic value was:     ");
              for (int i = 0; i < value.length(); i++)
              {
                Serial.print(byte(value[i]),HEX);
                if (i < value.length()-1)
                  Serial.print("-");
                  else Serial.println("");
              }
            }
          }
        }
        if (scharacteristic == Char2A25.toString().c_str())
        {
          if (Ok_Print_Decouverte) Serial.println ("   2A25                                                                                     ok repéré!");
          Characteristic2A25 = Service180A->getCharacteristic(Char2A25);
          if (Characteristic2A25->canBroadcast())       scharacteristic = scharacteristic + "[B]";
          if (Characteristic2A25->canIndicate())        scharacteristic = scharacteristic + "[I]";
          if (Characteristic2A25->canNotify())          scharacteristic = scharacteristic + "[N]";
          if (Characteristic2A25->canRead())            scharacteristic = scharacteristic + "[R]";
          if (Characteristic2A25->canWrite())           scharacteristic = scharacteristic + "[W]";
          if (Characteristic2A25->canWriteNoResponse()) scharacteristic = scharacteristic + "[WnR]";
          if (Ok_Print_Decouverte) Serial.print("   "+scharacteristic);
          if (Characteristic2A25->canRead())
          {
            std::string value = Characteristic2A25->readValue();
            if (Ok_Print_Decouverte) 
            {
              Serial.print("           Longueur : ");
              Serial.print(value.length());
              Serial.print("   ");
              Serial.print(value.c_str());
              Serial.print("           The Characteristic value was:     ");
              for (int i = 0; i < value.length(); i++)
              {
                Serial.print(byte(value[i]),HEX);
                if (i < value.length()-1)
                  Serial.print("-");
                  else Serial.println("");
              }
            }
          }  
        }
        if (scharacteristic == Char2A26.toString().c_str())
        {
          if (Ok_Print_Decouverte) Serial.println ("   2A26                                                                                     ok repéré!");
          Characteristic2A26 = Service180A->getCharacteristic(Char2A26);
          if (Characteristic2A26->canBroadcast())       scharacteristic = scharacteristic + "[B]";
          if (Characteristic2A26->canIndicate())        scharacteristic = scharacteristic + "[I]";
          if (Characteristic2A26->canNotify())          scharacteristic = scharacteristic + "[N]";
          if (Characteristic2A26->canRead())            scharacteristic = scharacteristic + "[R]";
          if (Characteristic2A26->canWrite())           scharacteristic = scharacteristic + "[W]";
          if (Characteristic2A26->canWriteNoResponse()) scharacteristic = scharacteristic + "[WnR]";
          if (Ok_Print_Decouverte) Serial.print("   "+scharacteristic);
          if (Characteristic2A26->canRead())
          {
            std::string value = Characteristic2A26->readValue();
            if (Ok_Print_Decouverte) 
            {
              Serial.print("           Longueur : ");
              Serial.print(value.length());
              Serial.print("   ");
              Serial.print(value.c_str());
              Serial.print("           The Characteristic value was:     ");
              for (int i = 0; i < value.length(); i++)
              {
                Serial.print(byte(value[i]),HEX);
                if (i < value.length()-1)
                  Serial.print("-");
                  else Serial.println("");
              }
            }
          }  
        }
        if (scharacteristic == Char2A27.toString().c_str())
        {
          if (Ok_Print_Decouverte) Serial.println ("   2A27                                                                                     ok repéré!");
          Characteristic2A27 = Service180A->getCharacteristic(Char2A27);
          if (Characteristic2A27->canBroadcast())       scharacteristic = scharacteristic + "[B]";
          if (Characteristic2A27->canIndicate())        scharacteristic = scharacteristic + "[I]";
          if (Characteristic2A27->canNotify())          scharacteristic = scharacteristic + "[N]";
          if (Characteristic2A27->canRead())            scharacteristic = scharacteristic + "[R]";
          if (Characteristic2A27->canWrite())           scharacteristic = scharacteristic + "[W]";
          if (Characteristic2A27->canWriteNoResponse()) scharacteristic = scharacteristic + "[WnR]";
          if (Ok_Print_Decouverte) Serial.print("   "+scharacteristic);
          if (Characteristic2A27->canRead())
          {
            std::string value = Characteristic2A27->readValue();
            if (Ok_Print_Decouverte) 
            {
              Serial.print("           Longueur : ");
              Serial.print(value.length());
              Serial.print("   ");
              Serial.print(value.c_str());
              Serial.print("           The Characteristic value was:     ");
              for (int i = 0; i < value.length(); i++)
              {
                Serial.print(byte(value[i]),HEX);
                if (i < value.length()-1)
                  Serial.print("-");
                  else Serial.println("");
              }
            }
          }  
        }
        if (scharacteristic == Char2A28.toString().c_str())
        {
          if (Ok_Print_Decouverte) Serial.println ("   2A28                                                                                     ok repéré!");
          Characteristic2A28 = Service180A->getCharacteristic(Char2A28);
          if (Characteristic2A28->canBroadcast())       scharacteristic = scharacteristic + "[B]";
          if (Characteristic2A28->canIndicate())        scharacteristic = scharacteristic + "[I]";
          if (Characteristic2A28->canNotify())          scharacteristic = scharacteristic + "[N]";
          if (Characteristic2A28->canRead())            scharacteristic = scharacteristic + "[R]";
          if (Characteristic2A28->canWrite())           scharacteristic = scharacteristic + "[W]";
          if (Characteristic2A28->canWriteNoResponse()) scharacteristic = scharacteristic + "[WnR]";
          if (Ok_Print_Decouverte) Serial.print("   "+scharacteristic);
          if (Characteristic2A28->canRead())
          {
            std::string value = Characteristic2A28->readValue();
            if (Ok_Print_Decouverte) 
            {
              Serial.print("           Longueur : ");
              Serial.print(value.length());
              Serial.print("   ");
              Serial.print(value.c_str());
              Serial.print("           The Characteristic value was:     ");
              for (int i = 0; i < value.length(); i++)
              { 
                Serial.print(byte(value[i]),HEX);
                if (i < value.length()-1)
                  Serial.print("-");
                  else Serial.println("");
              }
            }
          }  
        }
        if (scharacteristic == Char2A29.toString().c_str())
        {
          if (Ok_Print_Decouverte) Serial.println ("   2A29                                                                                     ok repéré!");
          Characteristic2A29 = Service180A->getCharacteristic(Char2A29);
          if (Characteristic2A29->canBroadcast())       scharacteristic = scharacteristic + "[B]";
          if (Characteristic2A29->canIndicate())        scharacteristic = scharacteristic + "[I]";
          if (Characteristic2A29->canNotify())          scharacteristic = scharacteristic + "[N]";
          if (Characteristic2A29->canRead())            scharacteristic = scharacteristic + "[R]";
          if (Characteristic2A29->canWrite())           scharacteristic = scharacteristic + "[W]";
          if (Characteristic2A29->canWriteNoResponse()) scharacteristic = scharacteristic + "[WnR]";
          if (Ok_Print_Decouverte) Serial.print("   "+scharacteristic);
          if (Characteristic2A29->canRead())
          {
            std::string value = Characteristic2A29->readValue();
            if (Ok_Print_Decouverte) 
            {
              Serial.print("           Longueur : ");
              Serial.print(value.length());
              Serial.print("   ");
              Serial.print(value.c_str());
              Serial.print("           The Characteristic value was:     ");
              for (int i = 0; i < value.length(); i++)
              {
                Serial.print(byte(value[i]),HEX);
                if (i < value.length()-1)
                  Serial.print("-");
                  else Serial.println("");
              }
            }
          }  
        }
        if (scharacteristic == Char2A2A.toString().c_str())
        {
          if (Ok_Print_Decouverte) Serial.println ("   2A2A                                                                                     ok repéré!");
          Characteristic2A2A = Service180A->getCharacteristic(Char2A2A);
          if (Characteristic2A2A->canBroadcast())       scharacteristic = scharacteristic + "[B]";
          if (Characteristic2A2A->canIndicate())        scharacteristic = scharacteristic + "[I]";
          if (Characteristic2A2A->canNotify())          scharacteristic = scharacteristic + "[N]";
          if (Characteristic2A2A->canRead())            scharacteristic = scharacteristic + "[R]";
          if (Characteristic2A2A->canWrite())           scharacteristic = scharacteristic + "[W]";
          if (Characteristic2A2A->canWriteNoResponse()) scharacteristic = scharacteristic + "[WnR]";
          if (Ok_Print_Decouverte) Serial.print("   "+scharacteristic);
          if (Characteristic2A2A->canRead())
          {
            std::string value = Characteristic2A2A->readValue();
            if (Ok_Print_Decouverte) 
            {
              Serial.print("           Longueur : ");
              Serial.print(value.length());
              Serial.print("   ");
              Serial.print(value.c_str());
              Serial.print("           The Characteristic value was:     ");
              for (int i = 0; i < value.length(); i++)
              {
                Serial.print(byte(value[i]),HEX);
                if (i < value.length()-1)
                  Serial.print("-");
                  else Serial.println("");
              }
            }
          }  
        }
        if (scharacteristic == Char2A50.toString().c_str())
        {
          if (Ok_Print_Decouverte) Serial.println ("   2A50                                                                                     ok repéré!");
          Characteristic2A50 = Service180A->getCharacteristic(Char2A50);
          if (Characteristic2A50->canBroadcast())       scharacteristic = scharacteristic + "[B]";
          if (Characteristic2A50->canIndicate())        scharacteristic = scharacteristic + "[I]";
          if (Characteristic2A50->canNotify())          scharacteristic = scharacteristic + "[N]";
          if (Characteristic2A50->canRead())            scharacteristic = scharacteristic + "[R]";
          if (Characteristic2A50->canWrite())           scharacteristic = scharacteristic + "[W]";
          if (Characteristic2A50->canWriteNoResponse()) scharacteristic = scharacteristic + "[WnR]";
          if (Ok_Print_Decouverte) Serial.print("   "+scharacteristic);
          if (Characteristic2A50->canRead())
          {
            std::string value = Characteristic2A50->readValue();
            if (Ok_Print_Decouverte) 
            {
              Serial.print("           Longueur : ");
              Serial.print(value.length());
              Serial.print("   ");
              Serial.print(value.c_str());
              Serial.print("           The Characteristic value was:     ");
              for (int i = 0; i < value.length(); i++)
              {
                Serial.print(byte(value[i]),HEX);
                if (i < value.length()-1)
                  Serial.print("-");
                  else Serial.println("");
              }
            }
          }  
        } 
      }
    }
    if (sservice == Serv18D0.toString().c_str())
    {
      if (Ok_Print_Decouverte) Serial.println (" 18D0                                                                                     ok repéré!");
      Service18D0 = pClient->getService(Serv18D0);
      std::map<std::string, BLERemoteCharacteristic*>* CharacteristicsMap = Service18D0-> getCharacteristics();
      if (Ok_Print_Decouverte) Serial.println("   Characteristic : " + String(CharacteristicsMap->size()));  
      for (auto &myCharacteristicEntry : *CharacteristicsMap)   
      {
        BLERemoteCharacteristic* chr = myCharacteristicEntry.second;
        if (Ok_Print_Decouverte) Serial.printf("   Characteristic found on Device: %s \n",  chr->getUUID().toString().c_str());
        String scharacteristic = chr->getUUID().toString().c_str();
        if (scharacteristic == Char2D00.toString().c_str())
        {
          if (Ok_Print_Decouverte) Serial.println ("   2D00                                                                                     ok repéré!");
          Characteristic2D00 = Service18D0->getCharacteristic(Char2D00);
          if (Characteristic2D00->canBroadcast())       scharacteristic = scharacteristic + "[B]";
          if (Characteristic2D00->canIndicate())        scharacteristic = scharacteristic + "[I]";
          if (Characteristic2D00->canNotify())          scharacteristic = scharacteristic + "[N]";
          if (Characteristic2D00->canRead())            scharacteristic = scharacteristic + "[R]";
          if (Characteristic2D00->canWrite())           scharacteristic = scharacteristic + "[W]";
          if (Characteristic2D00->canWriteNoResponse()) scharacteristic = scharacteristic + "[WnR]";
          if (Ok_Print_Decouverte) Serial.print("   "+scharacteristic);
          if (Characteristic2D00->canRead())
          {
            std::string value = Characteristic2D00->readValue();
            if (Ok_Print_Decouverte) 
            {
              Serial.print("           Longueur : ");
              Serial.print(value.length());
              Serial.print("   ");
              Serial.print(value.c_str());
              Serial.print("           The Characteristic value was:     ");
              for (int i = 0; i < value.length(); i++)
              {
                Serial.print(byte(value[i]),HEX);
                if (i < value.length()-1)
                  Serial.print("-");
                  else Serial.println("");
              }
            }
          }  
          std::map<std::string, BLERemoteDescriptor*>* DescriptorsMap = Characteristic2D00-> getDescriptors();
          if (Ok_Print_Decouverte) Serial.println("      Descriptor: " + String(DescriptorsMap->size()));
          for (auto &myDescriptorEntry : *DescriptorsMap)   
          {
            BLERemoteDescriptor* des = myDescriptorEntry.second;
            if (Ok_Print_Decouverte) Serial.printf("      Descriptor found on Device: %s \n",  des->getUUID().toString().c_str());
            DescriptorUUID = des->getUUID();
            String sdescriptor = des->getUUID().toString().c_str();
            if (sdescriptor == "00002901-0000-1000-8000-00805f9b34fb")
            {
              Descriptor2D00_2901 = Characteristic2D00->getDescriptor(DescriptorUUID);
              std::string value = Descriptor2D00_2901->readValue();
              if (Ok_Print_Decouverte) 
              {
                Serial.print("           Longueur : ");
                Serial.print(value.length());
                Serial.print("           The Descriptor value was: ");
                String s = value.c_str();
                Serial.print(s);
                Serial.print("           The Descriptor value was: ");
                Serial.print(" ");
                for (int i = 0; i < value.length(); i++)
                {
                  Serial.print(byte(value[i]),HEX);
                  if (i < value.length()-1)
                  {
                    Serial.print("-");
                  }
                  else Serial.println("");
                }
              }  
              Serial.println(" ");
            }
            if (sdescriptor == "00002902-0000-1000-8000-00805f9b34fb")
            {
              Descriptor2D00_2902 = Characteristic2D00->getDescriptor(DescriptorUUID);
              Characteristic2D00->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
              if(Characteristic2D00->canNotify())
                 Characteristic2D00->registerForNotify(notifyCallback2D00);
              std::string value = Descriptor2D00_2902->readValue();
              if (Ok_Print_Decouverte) 
              {
                Serial.print("           Longueur : ");
                Serial.print(value.length());
                Serial.print("           The Descriptor value was: ");
                String s = value.c_str();
                Serial.print(s);
                Serial.print("           The Descriptor value was: ");
                Serial.print(" ");
                for (int i = 0; i < value.length(); i++)
                {
                  Serial.print(byte(value[i]),HEX);
                  if (i < value.length()-1)
                  {
                    Serial.print("-");
                  }
                  else Serial.println("");
                }
                Serial.println(" ");
              }
            }
          }  
        }
        if (scharacteristic == Char2D01.toString().c_str())
        {
          if (Ok_Print_Decouverte) Serial.println ("   2D01                                                                                     ok repéré!");
          Characteristic2D01 = Service18D0->getCharacteristic(Char2D01);
          if (Characteristic2D01->canBroadcast())       scharacteristic = scharacteristic + "[B]";
          if (Characteristic2D01->canIndicate())        scharacteristic = scharacteristic + "[I]";
          if (Characteristic2D01->canNotify())          scharacteristic = scharacteristic + "[N]";
          if (Characteristic2D01->canRead())            scharacteristic = scharacteristic + "[R]";
          if (Characteristic2D01->canWrite())           scharacteristic = scharacteristic + "[W]";
          if (Characteristic2D01->canWriteNoResponse()) scharacteristic = scharacteristic + "[WnR]";
          if (Ok_Print_Decouverte) Serial.print("   "+scharacteristic);
          if (Characteristic2D01->canRead())
          {
            std::string value = Characteristic2D01->readValue();
            if (Ok_Print_Decouverte) 
            {
              Serial.print("           Longueur : ");
              Serial.print(value.length());
              Serial.print("   ");
              Serial.print(value.c_str());
              Serial.print("           The Characteristic value was:     ");
              for (int i = 0; i < value.length(); i++)
              {
                Serial.print(byte(value[i]),HEX);
                if (i < value.length()-1)
                  Serial.print("-");
                  else Serial.println("");
              }
            }
          }  
          std::map<std::string, BLERemoteDescriptor*>* DescriptorsMap = Characteristic2D01-> getDescriptors();
          if (Ok_Print_Decouverte) Serial.println("      Descriptor: " + String(DescriptorsMap->size()));
          for (auto &myDescriptorEntry : *DescriptorsMap)   
          {
            BLERemoteDescriptor* des = myDescriptorEntry.second;
            if (Ok_Print_Decouverte) Serial.printf("      Descriptor found on Device: %s \n",  des->getUUID().toString().c_str());
            DescriptorUUID = des->getUUID();
            String sdescriptor = des->getUUID().toString().c_str();
            if (sdescriptor == "00002901-0000-1000-8000-00805f9b34fb")
            {
              Descriptor2D01_2901 = Characteristic2D01->getDescriptor(DescriptorUUID);
              std::string value = Descriptor2D01_2901->readValue();
              if (Ok_Print_Decouverte) 
              {
                Serial.print("           Longueur : ");
                Serial.print(value.length());
                Serial.print("           The Descriptor value was: ");
                String s = value.c_str();
                Serial.print(s);
                Serial.print("           The Descriptor value was: ");
                Serial.print(" ");
                for (int i = 0; i < value.length(); i++)
                {
                  Serial.print(byte(value[i]),HEX);
                  if (i < value.length()-1)
                  {
                    Serial.print("-");
                  }
                  else Serial.println("");
                }  
                Serial.println(" ");
              }
            }
          }  
        }
      }
    }  
    if (sservice == ServFF01.toString().c_str())
    {
      if (Ok_Print_Decouverte) Serial.println (" FF01                                                                                     ok repéré!");
      ServiceFF01 = pClient->getService(ServFF01);
      std::map<std::string, BLERemoteCharacteristic*>* CharacteristicsMap = ServiceFF01-> getCharacteristics();
      if (Ok_Print_Decouverte) Serial.println("   Characteristic : " + String(CharacteristicsMap->size()));  
      for (auto &myCharacteristicEntry : *CharacteristicsMap)   
      {
        BLERemoteCharacteristic* chr = myCharacteristicEntry.second;
        if (Ok_Print_Decouverte) Serial.printf("   Characteristic found on Device: %s \n",  chr->getUUID().toString().c_str());
        String scharacteristic = chr->getUUID().toString().c_str();
        if (scharacteristic == CharFF02.toString().c_str())
        {
          if (Ok_Print_Decouverte) Serial.println ("   FF02                                                                                     ok repéré!");
          CharacteristicFF02 = ServiceFF01->getCharacteristic(CharFF02);
          if (CharacteristicFF02->canBroadcast())       scharacteristic = scharacteristic + "[B]";
          if (CharacteristicFF02->canIndicate())        scharacteristic = scharacteristic + "[I]";
          if (CharacteristicFF02->canNotify())          scharacteristic = scharacteristic + "[N]";
          if (CharacteristicFF02->canRead())            scharacteristic = scharacteristic + "[R]";
          if (CharacteristicFF02->canWrite())           scharacteristic = scharacteristic + "[W]";
          if (CharacteristicFF02->canWriteNoResponse()) scharacteristic = scharacteristic + "[WnR]";
          if (Ok_Print_Decouverte) Serial.println("   "+scharacteristic);
        }
        if (scharacteristic == CharFF03.toString().c_str())
        {
          if (Ok_Print_Decouverte) Serial.println ("   FF03                                                                                     ok repéré!");
          CharacteristicFF03 = ServiceFF01->getCharacteristic(CharFF03);
          if (CharacteristicFF03->canBroadcast())       scharacteristic = scharacteristic + "[B]";
          if (CharacteristicFF03->canIndicate())        scharacteristic = scharacteristic + "[I]";
          if (CharacteristicFF03->canNotify())          scharacteristic = scharacteristic + "[N]";
          if (CharacteristicFF03->canRead())            scharacteristic = scharacteristic + "[R]";
          if (CharacteristicFF03->canWrite())           scharacteristic = scharacteristic + "[W]";
          if (CharacteristicFF03->canWriteNoResponse()) scharacteristic = scharacteristic + "[WnR]";
          if (Ok_Print_Decouverte) Serial.print("   "+scharacteristic);
          std::map<std::string, BLERemoteDescriptor*>* DescriptorsMap = CharacteristicFF03-> getDescriptors();
          if (Ok_Print_Decouverte) Serial.println("      Descriptor: " + String(DescriptorsMap->size()));
          for (auto &myDescriptorEntry : *DescriptorsMap)   
          {
            BLERemoteDescriptor* des = myDescriptorEntry.second;
            if (Ok_Print_Decouverte) Serial.printf("      Descriptor found on Device: %s \n",  des->getUUID().toString().c_str());
            DescriptorUUID = des->getUUID();
            String sdescriptor = des->getUUID().toString().c_str();
            if (sdescriptor == "00002902-0000-1000-8000-00805f9b34fb")
            {
              DescriptorFF03_2902 = CharacteristicFF03->getDescriptor(DescriptorUUID);
              CharacteristicFF03->getDescriptor(BLEUUID((uint16_t)0x2902))->writeValue((uint8_t*)notificationOn, 2, true);
              if(CharacteristicFF03->canNotify())
                 CharacteristicFF03->registerForNotify(notifyCallbackFF03);
              std::string value = DescriptorFF03_2902->readValue();
              if (Ok_Print_Decouverte) 
              {
                Serial.print("           Longueur : ");
                Serial.print(value.length());
                Serial.print("           The Descriptor value was: ");
                String s = value.c_str();
                Serial.print(s);
                Serial.print("           The Descriptor value was: ");
                Serial.print(" ");
                for (int i = 0; i < value.length(); i++)
                {
                  Serial.print(byte(value[i]),HEX);
                  if (i < value.length()-1)
                  {
                    Serial.print("-");
                  }
                  else Serial.println("");
                }
                Serial.println(" ");
              }
            }
          }  
        }
      }
    }  
  }  
  connected = true;
  return true;
}

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class MyAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (Ok_Print_Scan) Serial.print("BLE Advertised Device found: ");
    if (Ok_Print_Scan) Serial.println(advertisedDevice.toString().c_str());
    Server_BLE_Address = new BLEAddress(advertisedDevice.getAddress());
    Scaned_BLE_Address = Server_BLE_Address->toString().c_str();
    if (Scaned_BLE_Address == My_BLE_Address)
    {
      Server_BLE_Address_ok = new BLEAddress(advertisedDevice.getAddress());
      Scaned_BLE_Address_ok = My_BLE_Address;
      BLEDevice::getScan()->stop();
      myDevice = new BLEAdvertisedDevice(advertisedDevice);
      doConnect = true;
      doScan = true;
  }
  } // onResult
}; // MyAdvertisedDeviceCallbacks

char CheckSum(std::string Str)
{
int iArr[Str.length()];
int i = Str.length();  
  for (int i2 = 0; i2<i; i2++)
  {
    iArr[i2] = byte(Str[i2]) & 255;
  }
  byte b2 = 0;
  int i3 = 0;
  while (b2 < i) {
    int i4 = i3;
    for (byte b3 = 0; b3 < 8; b3 = (byte) (b3 + 1)) {
      i4 = ((iArr[b2] ^ i4) & 1) == 1 ? ((i4 ^ 24) >> 1) | 128 : i4 >> 1;
      iArr[b2] = iArr[b2] >> 1;
    }
    b2 = (byte) (b2 + 1);
    i3 = i4;
  }
  return char(uint8_t(i3));
}

void Init_SB7()
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(04);        //0x0b
    newvalue = newvalue + char(00);        //0x04
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  Init_SB7 : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00) {delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    Lecture_2D00(Ok_Print_2D00);
  }
}  // end Init_SB7

void Batterie(byte &niveau)
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(05);        //0x0b
    newvalue = newvalue + char(03);        //0x04
    newvalue = newvalue + char(00);        //0x04
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  Batterie : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    String value = Lecture_2D00(Ok_Print_2D00);
    niveau = byte(value[3]);
  }
}  // end Batterie

void Recherche_SB7()
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(04);        //0x04
    newvalue = newvalue + char(9);         //0x09
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  Recherche_SB7 : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    Lecture_2D00(Ok_Print_2D00);
  }
}  // end Recherche_SB7

void Demande_Data_Perso(byte &genre, byte &taille, byte &poids, byte &age)
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(05);        //0x05
    newvalue = newvalue + char(06);        //0x06
    newvalue = newvalue + char(00);        //0x00
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  Demande Data Perso : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    String value = Lecture_2D00(Ok_Print_2D00);
    genre = byte(value[4]);
    taille = byte(value[5]);
    poids = byte(value[6]);
    age = byte(value[7]);
  }
}  // end Valide

void Langage()
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(05);        //0x05
    newvalue = newvalue + char(33);        //0x21
    newvalue = newvalue + char(00);        //0x00 00-> Europe, 01-> Asie
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  Langage : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    Lecture_2D00(Ok_Print_2D00);
  }
}  // end Demande

void Demande_Unite(byte &masse, byte &heure)
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(05);        //0x05
    newvalue = newvalue + char(02);        //0x02
    newvalue = newvalue + char(00);        //0x00
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  Demande Unité : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    String value = Lecture_2D00(Ok_Print_2D00);
    masse = byte(value[6]);
    heure = byte(value[5]);
  }
}  // end Confirme

void Demande_Date_Time(byte &an, byte &mois, byte &jour, byte &heure, byte &minute, byte &seconde)
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(05);        //0x05
    newvalue = newvalue + char(04);        //0x04
    newvalue = newvalue + char(00);        //0x00
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  Demande Date : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    String value = Lecture_2D00(Ok_Print_2D00);
    an = byte(value[4]);
    mois = byte(value[5]);
    jour = byte(value[6]);
    heure = byte(value[7]);
    minute = byte(value[8]);
    seconde = byte(value[9]);
  }
}  // end Confirme

void Fin_Demande()
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(05);        //0x05
    newvalue = newvalue + char(57);        //0x39
    newvalue = newvalue + char(00);        //0x00
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  Fin_Demande : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    String value = Lecture_2D00(Ok_Print_2D00);
    if (byte(value[5]) == 0)
      Serial.println(" Le système horaire est sur 24h ");
      else Serial.println(" Le système horaire est sur 12h ");
    if (byte(value[6]) == 0)
      Serial.println(" Le système métrique est cm/kg ");
      else Serial.println(" Le système métrique est Inch/Lb ");
  }
}  // end Fin_Demande

void UniteSI()
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(8);         //0x08
    newvalue = newvalue + char(02);        //0x02
    newvalue = newvalue + char(01);        //0x01
    newvalue = newvalue + char(00);        //0x00
    newvalue = newvalue + char(00);        //0x00 => Système 24 h
    newvalue = newvalue + char(00);        //0x00 => Système cm/kg
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  Unite_SI : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    Lecture_2D00(Ok_Print_2D00);
  }
}  // end Valide


void Date_Time(byte an, byte mois, byte jour, byte heure, byte minute, byte seconde)
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(11);        //0x0b
    newvalue = newvalue + char(04);        //0x04
    newvalue = newvalue + char(01);        //0x01
    newvalue = newvalue + char(an);        //an + 2000
    newvalue = newvalue + char(mois);      //mois
    newvalue = newvalue + char(jour);      //jour
    newvalue = newvalue + char(heure);     //heure
    newvalue = newvalue + char(minute);    //minute
    newvalue = newvalue + char(seconde);   //seconde
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  Date_Time : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    Lecture_2D00(Ok_Print_2D00);
  }
}  // end Date_Time

void Envoi_Message(String s)
{
  int longueur;
  if (s.length() > 13)
    longueur = 13;
    else longueur = s.length();
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(longueur + 7);        //0x13
    newvalue = newvalue + char(23);        //0x17
    newvalue = newvalue + char(0x08);        //0x01  / 00 bip, 01 logo téléphone bleu, 02 logo message jaune, 03 bip sans rien afficher, 04 logo nuage bleu, 05 bip sans afficher
                                                   // 06 bip, 07 bip, 08 bulles vertes, 16 bip
    newvalue = newvalue + char(01);        //0x01  /01 ok, autre ne fait rien
    newvalue = newvalue + char(01);        //0x01  /01 ok , autre ne fait rien
    if (s.length() > 13)
      longueur = 13;
      else longueur = s.length();
    for (int i = 0; i < longueur; i++)
      newvalue = newvalue + s[i];
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  Envoi Message : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
  }
}  // end Envoi_Message

void Data_Perso(byte genre, byte taille, byte poids, byte age)
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(9);         //0x09
    newvalue = newvalue + char(06);        //0x06
    newvalue = newvalue + char(01);        //0x01
    newvalue = newvalue + char(genre);     //genre 00 : homme, 01 : femme
    newvalue = newvalue + char(taille);    //taille
    newvalue = newvalue + char(poids);     //poids
    newvalue = newvalue + char(age);       //heure
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  Data_Perso : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    Lecture_2D00(Ok_Print_2D00);
  }
}  // end Data_Perso

void Reveil(byte n_reveil, byte jour, byte on_off, byte heure, byte minute)
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(12);        //0x0c
    newvalue = newvalue + char(05);        //0x05
    newvalue = newvalue + char(01);        //0x01
    newvalue = newvalue + char(n_reveil);  //N_reveil : n° réveil de 00 à 04
    newvalue = newvalue + char(on_off);    // 01 : ok, 00 : éteint
    newvalue = newvalue + char(01);        //0x01
    newvalue = newvalue + char(10);        //0x0A
    newvalue = newvalue + char(jour);      //jour de fonctionnement 0/D/L/Ma/Me/J/V/S (1 octet)
    newvalue = newvalue + char(heure);     //heure
    newvalue = newvalue + char(minute);    //minute
    newvalue = newvalue + CheckSum(newvalue);  //chksum
    if (Ok_Print)
      Serial.print("  Réveil : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    Lecture_2D00(Ok_Print_2D00);
  }
}  // end Reveil

void Demande_Reveil()
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(05);        //0x0c
    newvalue = newvalue + char(05);        //0x05
    newvalue = newvalue + char(00);        //0x01
    newvalue = newvalue + CheckSum(newvalue);  //chksum
    if (Ok_Print)
      Serial.print("  Réveil : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    Lecture_2D00(Ok_Print_2D00);
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    Lecture_2D00(Ok_Print_2D00);
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    Lecture_2D00(Ok_Print_2D00);
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    Lecture_2D00(Ok_Print_2D00);
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    Lecture_2D00(Ok_Print_2D00);
  }
}  // end Demande Reveil

void BPM(byte &battement)
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(05);        //0x05
    newvalue = newvalue + char(15);        //0x0f
    newvalue = newvalue + char(01);        //0x01
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  BPM : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    String value = Lecture_2D00(Ok_Print_2D00);
//    while ((value[2] != char(16)) or (value[3] != char(1))) {value = Lecture_2D00(Ok_Print_2D00);}
    battement = byte(value[4]);
  }
}  // end BPM()

void TA(byte &psys, byte &pdias)
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(05);        //0x05
    newvalue = newvalue + char(15);        //0x0f
    newvalue = newvalue + char(02);        //0x02
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  TA : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    String value = Lecture_2D00(Ok_Print_2D00);
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    value = Lecture_2D00(Ok_Print_2D00);
//    while ((value[2] != char(16)) or (value[3] != char(2))) {value = Lecture_2D00(Ok_Print_2D00);}
    psys = byte(value[4]);
    pdias = byte(value[5]);
  }
}  // end TA()

void SPO2(byte &spo2)
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(05);        //0x05
    newvalue = newvalue + char(15);        //0x0f
    newvalue = newvalue + char(04);        //0x04
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  SPO2 : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    String value = Lecture_2D00(Ok_Print_2D00);
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    value = Lecture_2D00(Ok_Print_2D00);
//    while ((value[2] != char(16)) or (value[3] != char(4))) {value = Lecture_2D00(Ok_Print_2D00);}
    spo2 = byte(value[4]);
  }
}  // end SPO2()

void Demande_Nombre_Pas_A_Faire(unsigned int &nbre_pas_a_faire)   
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(5);         //0x05
    newvalue = newvalue + char(37);        //0x25
    newvalue = newvalue + char(00);        //0x01
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  Demande Nombre Pas à Faire : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    String value = Lecture_2D00(Ok_Print_2D00);
    nbre_pas_a_faire = byte(value[6])*256+byte(value[7]);
  }
}  // end Demande_Nbre_Pas_A_Faire

void Init_Nombre_Pas_A_Faire(unsigned int nbre_pas_a_faire)   
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(9);         //0x09
    newvalue = newvalue + char(37);        //0x25
    newvalue = newvalue + char(01);        //0x01
    newvalue = newvalue + char(00);        
    newvalue = newvalue + char(00);        
    newvalue = newvalue + char(nbre_pas_a_faire >> 8);        
    newvalue = newvalue + char(nbre_pas_a_faire & 0x00FF);        
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  Init Nombre Pas à Faire : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    String value = Lecture_2D00(Ok_Print_2D00);
  }
}  // end Init_Nbre_Pas_A_Faire

void Demande_13()   
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(5);         //0x05
    newvalue = newvalue + char(19);        //0x25
    newvalue = newvalue + char(00);        //0x01
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  Demande 13 : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    String value = Lecture_2D00(Ok_Print_2D00);
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    value = Lecture_2D00(Ok_Print_2D00);
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    value = Lecture_2D00(Ok_Print_2D00);
  }
}  // end Demande_13

void Demande_X()   
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(5);         //0x05
    newvalue = newvalue + char(00);        //0x25
    newvalue = newvalue + char(00);        //0x01
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  Demande X : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    String value = Lecture_2D00(Ok_Print_2D00);
  }
}  // end Demande_X

void Demande_Nbre_Pas(byte &nbre_pas)   
{
  if (connected) {
    std::string newvalue;
    newvalue ="";
    newvalue = newvalue + char(171);       //0xab
    newvalue = newvalue + char(5);         //0x05
    newvalue = newvalue + char(18);        //0x25
    newvalue = newvalue + char(00);        //0x01
    newvalue = newvalue + CheckSum(newvalue);  // chksum
    if (Ok_Print)
      Serial.print("  Demande Nombre Pas Fait : ");
    Ecrire_2D01(newvalue, Ok_Print_2D01);
    if (Ok_Print &!Ok_Print_2D01)
      Serial.println(" ");
    while (!Donnee_prete_2D00){delayMicroseconds(10);}
    Donnee_prete_2D00 = false;
    String value = Lecture_2D00(Ok_Print_2D00);
    nbre_pas = byte(value[9])*256+byte(value[10]);
  }
}  // end Demande_Nombre_Pas

void Recherche_Montre()
{
  // Retrieve a Scanner and set the callback we want to use to be informed when we
  // have detected a new device.  Specify that we want active scanning and start the
  // scan to run for 5 seconds.
  BLEScan* pBLEScan = BLEDevice::getScan();
  pBLEScan->setAdvertisedDeviceCallbacks(new MyAdvertisedDeviceCallbacks());
  pBLEScan->setInterval(1349);
  pBLEScan->setWindow(449);
  pBLEScan->setActiveScan(true);
  pBLEScan->start(5, false);
}

void setup() {
  Serial.begin(115200);
  Serial.println("Starting Arduino BLE Client application...");
  BLEDevice::init("");
  Recherche_Montre();
} // End of setup.


// This is the Arduino main loop function.
void loop() {
  // If the flag "doConnect" is true then we have scanned for and found the desired
  // BLE Server with which we wish to connect.  Now we connect to it.  Once we are 
  // connected we set the connected flag to be true.
  if (doConnect == true) {
    if (connectToServer()) {
      Serial.println("We are now connected to the BLE Server.");
    } else {
      Serial.println("We have failed to connect to the server; there is nothin more we will do.");
    }
    doConnect = false;
  }
  // If we are connected to a peer BLE Server, update the characteristic each time we are reached
  // with the current time since boot.
  if (connected) {
    Serial.println("     ");
    Serial.println("     ");
    Serial.println("     ");
    Serial.println("     ");
    std::string value = Characteristic2A00->readValue();
    Serial.printf("   La montre est une : %s \n", value.c_str());
    Serial.println("     ");
    
    Init_SB7(); 
    Serial.println("     ");
    delay(100);
    
    byte Niveau_Batterie;
    Batterie(Niveau_Batterie); // résultat en %
    Serial.print("   Le niveau de batterie est de : ");
    Serial.print(Niveau_Batterie);
    Serial.println("%      ");
    Serial.println("      ");
    delay(100);

    UniteSI();  
    Serial.println("     ");
    delay(100);

    byte Systeme_Metrique;
    byte Systeme_Horaire;
    Demande_Unite(Systeme_Metrique, Systeme_Horaire); 
    if (Systeme_Metrique == 0) 
      Serial.println("   Le système métrique est cm/kg ");
      else Serial.println("   Le système métrique est Inch/Lb ");
    if (Systeme_Horaire == 0) 
      Serial.println("   Le système horaire est sur 24h ");
      else Serial.println("   Le système horaire est sur 12h ");
    Serial.println("     ");
    delay(100);

    byte genre = 00;
    byte taille = 176;
    byte poids = 68;
    byte age = 62;
    Data_Perso(genre, taille, poids, age);   
    Serial.println("     ");
    delay(100);
    
    Demande_Data_Perso(genre, taille, poids, age); 
    Serial.print("   Le pratiquant est ");
    if (genre == 0)
      Serial.println("un homme");
      else Serial.println("une femme");
    Serial.print("   Le pratiquant mesure ");
    Serial.print(taille);
    Serial.println(" cm");
    Serial.print("   Le pratiquant pèse ");
    Serial.print(poids);
    Serial.println(" Kg");
    Serial.print("   Le pratiquant a ");
    Serial.print(age);
    Serial.println(" ans");
    Serial.println("     ");
    delay(100);

    byte annee = 21; //pour 2021
    byte mois = 12;  // pour décembre
    byte jour = 06;  
    byte heure = 13;
    byte minute = 40;
    byte seconde = 20;
/*    Date_Time(annee, mois, jour, heure, minute, seconde);   
    Serial.println("     ");
    delay(100);*/

    Demande_Date_Time(annee, mois, jour, heure, minute, seconde);
    Serial.print("   Nous sommes le ");
    Serial.print(jour);
    Serial.print("/");
    Serial.print(mois);
    Serial.print("/");
    Serial.print(annee);
    Serial.print(" à ");
    Serial.print(heure);
    Serial.print(":");
    Serial.print(minute);
    Serial.print(":");
    Serial.println(seconde);
    Serial.println("     ");
    delay(100);

    String s = "Bonjour0123456789ABCDEFGHIJKLMNOPQ";
    Envoi_Message(s);
    delay(2000);

/*    byte nbre_pas_fait;
    Demande_Nbre_Pas(nbre_pas_fait);
    Serial.print("   Le pratiquant a fait ");
    Serial.print(nbre_pas_fait);
    Serial.println(" pas ce jour");
    Serial.println("     ");
    delay(100);*/
    
/*    Serial.println("     ");
    Serial.println("     ");
    Demande_X();
    Serial.println("     ");
    Serial.println("     ");
    delay(100);*/
    
/*    unsigned int nbre_pas_a_faire = 65000; // de 4000 à 65000 tous les 500 pas
    Init_Nombre_Pas_A_Faire(nbre_pas_a_faire);   
    Serial.println("     ");
    delay(100);    

    Demande_Nombre_Pas_A_Faire(nbre_pas_a_faire);   
    Serial.print("   Le pratiquant doit faire ");
    Serial.print(nbre_pas_a_faire);
    Serial.println(" pas/jour");
    Serial.println("     ");
    delay(100);  */

/*    Serial.println("     ");
    Serial.println("     ");
    Demande_13();
    Serial.println("     ");
    Serial.println("     ");*/

    byte battement;
    BPM(battement);
    Serial.print("   Le pratiquant a une fréquence cardiaque de ");
    Serial.print(battement);
    Serial.println(" battements/mn");
    Serial.println("     ");
    delay(100);
  
/*    byte Psys;
    byte Pdias;
    TA(Psys, Pdias);
    Serial.print("   Le pratiquant a une Tension Artérielle estimée => Pression Systolique : ");
    Serial.print(Psys);
    Serial.print(" mmHg et une Pression diastolique de : ");
    Serial.print(Pdias);
    Serial.println(" mmHg");
    Serial.println("     ");
    delay(100);*/
    
/*    byte spo2;
    SPO2(spo2);
    Serial.print("   Le pratiquant a une concentration en oxygène dans le sang de : ");
    Serial.print(spo2);
    Serial.print("%");
    Serial.println("     ");
    delay(100); */

/*    Recherche_SB7();  
    Serial.println("     ");
    delay(2000); */

/*    Langage();   //  définit le langage en mode europééen ou asiatique
    Serial.println("     ");
    delay(100);

    Reveil(00,0x7F,01,20,57);  
    Serial.println("     ");
    delay(100);

    Reveil(00,0x7F,00,20,57);  
    Serial.println("     ");
    delay(100);*/

/* attention plante car passe très vite dans notify callback 
    Demande_Reveil();  // passe 5 fois dans notify callback
*/    

  }else Recherche_Montre();
/*  if(doScan){
    BLEDevice::getScan()->start(0);  // this is just eample to start scan after disconnect, most likely there is better way to do it in arduino
  }*/
  
  delay(1000); // Delay a second between loops.
} // End of loop

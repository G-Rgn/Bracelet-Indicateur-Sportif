/*
     Code pour manipuler la montre SB 7+
*/


#include <NimBLEDevice.h>

void scanEndedCB(NimBLEScanResults results);

/*
   Initialisation des variables
*/
static NimBLEAdvertisedDevice* advDevice;
static bool doConnect = false;
bool scanEnd = false;
static uint32_t scanTime = 5; /* 0 = scan forever */
String command;

std::vector<NimBLERemoteService*>* remoteServ;
std::vector<NimBLERemoteCharacteristic*>* remoteChar;
std::vector<NimBLERemoteDescriptor*>* remoteDesc;
NimBLEClient* clientBLE ;

/*
   Initialisation des variables
*/
class ClientCallbacks : public NimBLEClientCallbacks {
    void onConnect(NimBLEClient* pClient) {
      remoteServ = pClient->getServices(true);
      for (int i = 0; i < (*remoteServ).size(); i++) {
        Serial.println((*remoteServ).at(i)->getUUID().toString().c_str());
      }
      Serial.println((*remoteServ).size());
      Serial.println("Connected");

      /*  Changement des paramètres pour augmenter le temps de réponse possible de l'appareil
          Le timeout doit être un multiple de l'intervalle de requête, le minimum est 100ms.
          Multiplier par 3 à 5 * l'intervalle fonctionne bien pour des réponses/reconnexions rapides
          Min interval: 120 * 1.25ms = 150, Max interval: 120 * 1.25ms = 150, 0 latence, 60 * 10ms = 600ms timeout
      */
      pClient->updateConnParams(120, 120, 0, 60);
    };

    /*
       Lors de la déconnexion du client
    */
    void onDisconnect(NimBLEClient* pClient) {
      Serial.print(pClient->getPeerAddress().toString().c_str());
      Serial.println(" Disconnected - Starting scan");
      NimBLEDevice::getScan()->start(scanTime, scanEndedCB);
    };
};


/* Classe qui récupère les Callback de l'appareil connecté */
class AdvertisedDeviceCallbacks: public NimBLEAdvertisedDeviceCallbacks {

    void onResult(NimBLEAdvertisedDevice* advertisedDevice) {
      Serial.print("Advertised Device found: ");
      Serial.println(advertisedDevice->toString().c_str());

      if (advertisedDevice->getName() == "SB 7+")//si l'appareil découvert a commer nom SB 7+
      {
        Serial.println("Found Our Service");
        /* on coupe le scan pour se connecter */
        NimBLEDevice::getScan()->stop();
        NimBLEDevice::setMTU(185);
        Serial.println(NimBLEDevice::getMTU());

        /* on enregistre l'appareil*/
        advDevice = advertisedDevice;
        /* on peut lancer la connexion */
        doConnect = true;
      }
    };
};


/* Gestion des Notification / Indication issues des callbacks */
void notifyCB(NimBLERemoteCharacteristic* pRemoteCharacteristic, uint8_t* pData, size_t length, bool isNotify) {
  Serial.println("");
  /*
    Affiche la Notification/Indication reçue
  */
  std::string str = (isNotify == true) ? "Notif" : "Indication";
  str += " from ";

  str += std::string(pRemoteCharacteristic->getRemoteService()->getClient()->getPeerAddress());
  str += ": Service = " + std::string(pRemoteCharacteristic->getRemoteService()->getUUID());
  str += ", Characteristic = " + std::string(pRemoteCharacteristic->getUUID());
  str += ", Value = " + std::string((char*)pData, length);
  Serial.println(str.c_str());
  /*
     Affichage de la valeur sous un format de liste numérique
  */
  Serial.print("pData: ");
  for (int i = 0; i < length; i++) {
    Serial.print(*(pData + i));
    Serial.print("-");
  }
  Serial.println("");

  if (pRemoteCharacteristic->getUUID() == NimBLEUUID("00002d00-0000-1000-8000-00805f9b34fb")) {
    Serial.print("HEART RATE : ");
    Serial.print(*(pData + 4));
    Serial.println(" BPM");
    Serial.println("");
  }
}



/* Affiche la fin du scan */
void scanEndedCB(NimBLEScanResults results) {
  scanEnd = true;
  Serial.println("Scan Ended");
}


/* Create a single global instance of the callback class to be used by all clients */
static ClientCallbacks clientCB;


/* Gère la création du client et se connecte avec le serveur */
/* Recherche de clients existants **/
  if (NimBLEDevice::getClientListSize()) {
    /* Si l'appareil à connecté est déjà connu on se reconnecte
        Sinon on retourne false
    */
    pClient = NimBLEDevice::getClientByPeerAddress(advDevice->getAddress());
    if (pClient) {
      /*
         S'il est impossible de se connecter
      */
      if (!pClient->connect(advDevice, false)) {
        Serial.println("Reconnect failed");
        return false;
      }
      Serial.println("Reconnected client");
    }

    else {
      pClient = NimBLEDevice::getDisconnectedClient();
    }
  }

  /* Création d'un nouveau client */
  if (!pClient) {
    /* S'il y a trop de client enregistré */
    if (NimBLEDevice::getClientListSize() >= NIMBLE_MAX_CONNECTIONS) {
      Serial.println("Max clients reached - no more connections available");
      return false;
    }

    pClient = NimBLEDevice::createClient();

    Serial.println("New client created");

    pClient->setClientCallbacks(&clientCB, false);
    /* Changement des paramètres pour augmenter le temps de réponse possible de l'appareil
        Le timeout doit être un multiple de l'intervalle de requête, le minimum est 100ms.
        Multiplier par 3 à 5 * l'intervalle fonctionne bien pour des réponses/reconnexions rapides
        Min interval: 120 * 1.25ms = 150, Max interval: 120 * 1.25ms = 150, 0 latence, 60 * 10ms = 600ms timeout
    */
    /* Définition des paramètres de base de connexion : 15ms intervalle, 0 latence, 120ms timeout.
        Ces paramètres permettent de se connecter à 3 clients de manière fiable, s'il y en a moins, il y a possibilité d'augmenter la vitesse
        Le timeout doit être un multiple de l'intervalle, le minimum est 100ms.
        Min interval: 12 * 1.25ms = 15, Max interval: 12 * 1.25ms = 15, 0 latence, 51 * 10ms = 510ms timeout
    */
    pClient->setConnectionParams(12, 12, 0, 51);
    /* Temps d'attente pour la connexion, par défaut 30s. */
    pClient->setConnectTimeout(5);


    if (!pClient->connect(advDevice)) {
      /* Si échec de connexion */
      NimBLEDevice::deleteClient(pClient);
      Serial.println("Failed to connect, deleted client");
      return false;
    }
  }

  /* S'il l'appareil ne peut pas être connecté étant connu */
  if (!pClient->isConnected()) {
    if (!pClient->connect(advDevice)) {
      Serial.println("Failed to connect");
      return false;
    }
  }
  /* Affiche l'appareil auquel on est connecté */
  Serial.print("Connected to: ");
  Serial.println(pClient->getPeerAddress().toString().c_str());
  clientBLE = pClient; // enregistrement du client

  /* Création des services/caractéristiques/descripteur avec lesquels nous allons pouvois communiquer */
  NimBLERemoteService* pSvc = nullptr;
  NimBLERemoteCharacteristic* pChr = nullptr;
  NimBLERemoteDescriptor* pDsc = nullptr;

  /* On récupère chacuns des services */
  for (int i = 0; i < (*remoteServ).size(); i++) {
    pSvc = (*remoteServ).at(i);
    Serial.print("\n\nSERVICE ");
    Serial.println((*remoteServ).at(i)->getUUID().toString().c_str());

    /* Si le service n'est pas null */
    if (pSvc) {
      remoteChar = pSvc->getCharacteristics(true);
      /* Récupère chacunes des caractéristiques */
      for (int j = 0; j < (*remoteChar).size(); j++) {
        Serial.print("    ");
        Serial.print("CHARACTERISTIC ");
        Serial.println((*remoteChar).at(j)->getUUID().toString().c_str());
        pChr = (*remoteChar).at(j);

        /* Si la caractéristique n'est pas nulle */
        if (pChr) {
          /* Lorsqu'il est possible de lire la valeur on le fait */
          if (pChr->canRead()) {
            Serial.println("        CAN READ ");
            Serial.print("          Characteristic Value: ");
            Serial.println(pChr->readValue().c_str());
          }
          /* Lorsqu'il est possible d'écrire une valeur */
          if (pChr->canWrite()) {
            Serial.println("        CAN WRITE ");
          }

          /* Lorsqu'il peut être notifié */
          if (pChr->canNotify()) {
            Serial.println("       CAN NOTIFY ");
            /* Si on arrive à s'abonner aux notifications */
            if (pChr->subscribe(true, notifyCB, true)) {
              Serial.println("       SUBSCRIBED NOTIFY");
            }
            /* Si on arrive à s'enregistrer aux notifications */
            if (pChr->registerForNotify(notifyCB, true, true)) {
              Serial.println("       REGISTERED FOR NOTIFY");
            }
          }
          /* Lorsqu'il est peut être indiqué */
          else if (pChr->canIndicate()) {
            Serial.println("       CAN INDICATE ");
            /* Si on arrive à s'abonner aux indications */
            if (pChr->subscribe(false, notifyCB)) {
              Serial.println("       SUBSCRIBED INDICATE");
            }
          }

          remoteDesc = pChr->getDescriptors(true);
          /* Récupère chacun des descripteurs */
          for (int k = 0; k < (*remoteDesc).size(); k++) {
            Serial.print("      ");
            Serial.print("DESCRIPTOR ");
            Serial.println((*remoteDesc).at(k)->getUUID().toString().c_str());
            pDsc = (*remoteDesc).at(k);
            Serial.print("          Descriptor Value: ");
            Serial.println(pDsc->readValue().c_str());
            Serial.println(pDsc->readUInt8());
          }
        }
      }
    }
    else {
      Serial.println("Service not found.");
    }

  }
  Serial.println("Done with this device!");
  return true;
}


void setup () {
  /* Initialisation de la communication Série */
  Serial.begin(115200);
  Serial.println("Starting NimBLE Client");
  /* Initialisation de NimBLE, pas besoin de mettre de nom d'appareil */
  NimBLEDevice::init("");

  /* Optionel: défini la puissance de transmission, par défaut 3db */
  NimBLEDevice::setPower(ESP_PWR_LVL_P9); /* +9db */

  /* Réalisation d'un scan */
  NimBLEScan* pScan = NimBLEDevice::getScan();

  /* Création d'un callback lorsqu'un appareil est trouvé */
  pScan->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());

  /* Défini l'intervalle du scan (la fréquence) and la fenêtre de temps (durée) en millisecondes */
  pScan->setInterval(45);
  pScan->setWindow(15);

  /* Un scan actif va récupérer les données des appareils mais va demander plus d'énergie
  */
  pScan->setActiveScan(true);
  /* Début du scan sur une durée définie (en secondes) 0 = forever
      Callback optionnel lorsque le scan s'arrête.
  */
  pScan->start(scanTime, scanEndedCB);
}



void loop () {
  /* Tourne en boucle tant qu'aucun appareil à connecter est trouvé */
  while (!doConnect & !scanEnd) {
    delay(1);
  }

  /* Lorsqu'un appareil à connecter est trouvé */
  doConnect = false;

  /* Connexion à l'appareil */
  if (connectToServer()) {
    Serial.println("Success! we should now be getting notifications, scanning for more!");
  } else {
    Serial.println("Failed to connect, starting scan");
  }

  /* Lorsque le scan est fini et qu'on a trouvé un client */
  if (scanEnd && clientBLE != nullptr) {
    while (true) {
      /* Attente de la réception d'une commande*/
      if (Serial.available()) {
        command = Serial.readStringUntil('\n');
        command.trim();
        /* Mesure du rythme cardiaque */
        if (command.equals("heart")) {
          getHeartRate(clientBLE);
        }

      }
    }
  }
}



/* fonctions permettant de récupérer le rythme cardiaque */
void getHeartRate(NimBLEClient* pClient) {
  NimBLERemoteService* serviceHeart = nullptr;
  NimBLERemoteCharacteristic* chrHeart = nullptr;
  serviceHeart = pClient->getService("000018d0-0000-1000-8000-00805f9b34fb");

  Serial.println(serviceHeart->getUUID().toString().c_str());
  chrHeart = serviceHeart->getCharacteristic(NimBLEUUID("00002d01-0000-1000-8000-00805f9b34fb"));
  
  Serial.println(chrHeart->getUUID().toString().c_str());
  uint8_t notificationOn[] = {0xab, 0x05, 0x0f, 0x01, 0x4e}; //trame qui lance une mesure de rythme cardiaque

  if (chrHeart->writeValue((uint8_t*)notificationOn, 5, true)) {
    Serial.print("Wrote new value to: ");
    Serial.println(chrHeart->getUUID().toString().c_str());
  }
}

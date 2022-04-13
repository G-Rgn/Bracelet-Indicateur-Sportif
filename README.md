# Bracelet-Indicateur-Sportif
Solution pour utiliser des montres du marché sans connaître leur fonctionnement au départ.  
## Comprendre le fonctionnement de la communication Bluetooth entre le téléphone et la montre
### Méthode avec le sniffer BLEFriend
> Flash à réaliser si le produit n'est pas sous le format de sniffer
> Pour le [Flash](https://github.com/G-Rgn/Bracelet-Indicateur-Sportif/blob/ConnectESP32/flashcartesniffer.txt).

Installez le module selon les démarches indiquées dans l'[Installation](https://github.com/G-Rgn/Bracelet-Indicateur-Sportif/blob/ConnectESP32/installationSniffer.txt).  
Une fois l'installation réalisée, vous pouvez récupérer l'ensemble des données qui ont transitées entre l'appareil qui est tracé et l'extérieur.  
#### Les données à exploiter
Les données les plus intéressantes sont le service Bluetooth qui est sollicité, ainsi que les messages envoyé et reçus.  
La connexion Bluetooth et la création du serveur GATT se fait automatiquement grâce à la librairire [NimBLE-Arduino](https://github.com/h2zero/NimBLE-Arduino).  
Les trames tirées de la communication vont permettre d'apprendre comment le téléphone communique avec la montre et donc reproduire le même comportement avec l'ESP32.  
### Méthode avec le mode HCI sur un smartphone Android
#### Point obligatoire
Activer le mode développeur sur votre téléphone Android (Paramètres > A propos > Informations sur le logiciel > Cliquer plusieurs fois sur le numéro de Version
jusqu'à ce que votre téléphone vous indique l'activation de ce mode).  
Vous devez aussi télécharger adb sur votre ordinateur. [Lien de téléchargement ADB](https://dl.google.com/android/repository/platform-tools-latest-windows.zip)  
#### Récupérer les logs de la communication
Veuillez suivre la démarche indiquée dans le fichier correspondant pour avoir les [logs HCI](https://github.com/G-Rgn/Bracelet-Indicateur-Sportif/blob/ConnectESP32/hciBluetooth.txt).  
Une fois le fichier récupéré, ouvrez le avec Wireshark.
### Etude de l'application réservé au modèle de la montre
1. Téléchargez l'application souhaitée au format APK
2. Téléchargez [JADX](https://www.softpedia.com/dyn-postdownload.php/a5035ed4b84749d8d2d91517a3eabd3c/62569f05/38cdc/0/1)

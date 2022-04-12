# Bracelet-Indicateur-Sportif
Solution pour utiliser des montres du marché sans connaître leur fonctionnement au départ.  
## Méthode avec le sniffer BLEFriend
> Flash à réaliser si le produit n'est pas sous le format de sniffer

Installez le module selon les démarches indiquées dans les différents fichiers correspondants.  
Une fois l'installation réalisée, vous pouvez récupérer l'ensemble des données qui ont transitées entre l'appareil qui est tracé et l'extérieur.  
### Les données à exploiter
Les données les plus intéressantes sont le service Bluetooth qui est sollicité, ainsi que les messages envoyé et reçus.  
Dans la suite du protocole il y a la connexion qui se fait automatiquement grâce au fichier proposé à cet effet.  
Les trames tirées de la communication vont permettre d'apprendre comment le téléphone communique avec la montre et donc reproduire le même comportement avec l'ESP32.  
## Méthode avec le mode HCI sur un smartphone Android
### Point obligatoire
Activer le mode développeur sur votre téléphone Android (Paramètres > A propos > Informations sur le logiciel > Cliquer plusieurs fois sur le numéro de Version
jusqu'à ce que votre téléphone vous indique l'activation de ce mode).  
Vous devez aussi télécharger adb sur votre ordinateur. [Lien de téléchargement ADB](https://dl.google.com/android/repository/platform-tools-latest-windows.zip)  
### Récupérer les logs de la communication
Connectez-vous à votre téléphone avec adb puis suivez les démarches indiquées dans le fichier correspondant

# SPIDS
Smart Plantts IDS project: controling the environment of a plant with an Arduino system

## Visualisation des données
Les données recueillies par les différents capteurs peuvent être visualisées à l'aide d'un dashboard réalisé avec `Node-RED`. Pour cela nous utilisons un flow comme ci-dessous:   
![pic](assets/node-red-dashboard.png)

Le résultat dans le cas de l'humidité est le suivant:    
![dashboard_hum](assets/humidity_dashboard.png)

## Notifications
Un système de notifications automatisés est implémenté sur `Node-RED`. Il permet d'informer en temps réel l'utilisateur des variations de l'environnement de sa plante via sa messagerie électronique. 
![notification](assets/notification.png)    
![emails](assets/mails.png)



## Circuit

Voici un schéma du montage : 
![schéma](assets/capteurs_schema_unique_plaquette.PNG)
n.b.: le capteur d'hygrométrie n'étant pas disponible sur le logiciel de visualisation, nous l'avons remplacé par un capteur infrarouge (3 pins, un cercle au milieu)

Le lien vers le schéma, Tinkercad : https://www.tinkercad.com/things/bAPoRXGjClL-copy-of-montage-ids/editel?sharecode=ed_YTJ0ywndT22aBq9hvQLNpsKSYZYKvbhE34J7VrUU

## Circuits

Pour commander les actionneurs reliés à la carte `Arduino Uno` depuis la carrte `RaspberryPi` un système de messages codés est utilisé. Chaque message est composé de 6 chiffres, les deux premiers permettent d'identifier la commande et les 4 suivants la valeur associée. Par exemple: `"010018"` a pour code "01" ce qui permet dde régler la température minimale et la valeur est "0018" ce qui correspond à 18°C. 

| Commande               | Code | Info  |
|------------------------|------|-------|
| Température minimale   | 01   |en °C 		|
| Température maximale   | 02   |en °C		|
| Heure coucher 	 | 03   |hhmm 		|
| Heure lever 		 | 04   |hhmm		|
| Actionner la pompe     | 05   |peu importe 	|
| Actionner le chauffage | 06   |peu importe    |
| Humidité minimale      | 07   |une valeur entre 0000 et 1000|
| Changer mode pompe     | 08   |0000 if full manual, 0001 if hygrometric, 0002 if cyclic	|
| Changer mode chauffage | 09   |peu importe |
| Changer heure		 | 10   |hhmm (tant pis pour les secondes)|
| Changer time_pump	 | 11 	|en (ms)	|
| Changer etat de lampe  | 12   |0000 light off, 1111 light on|




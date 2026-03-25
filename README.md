# STM32_2026_G14
***overview*** : Ce dépôt github, contient l'ensemble des codes pour la réalisation du bureau d'étude.  


## Système d'Arrossage Automatique d'une plante Connectée et Communication Bluetooth avec le STM32WB55RG

### Schéma de Cablage du Système
![photo_cablage](Rapport/images_BE/projet.jpg)
### Machine état du Système
Les deux services du bluetooth fonctionnent en parallèle avec le système d'asservissement.
Le diagramme ci-dessous est une representation en l'état du fonctionnement globale du système.


![state_machine](Rapport/Machine_etat/system.png)

### Réalisation

## Montage sous Alimentation Externe 
On a connecté la carte sous l'alimentation stable de +3.7V par le générateur DC.
La consommation Electrique est de 3.7V*57.9mA = 0.21423W.
Ci dessous le montage : 
![montage_alimenation_externe](Rapport/images_BE/Alimentation_externe.jpeg)
La figure ci-dessus montre le montage finale et on peut y voir le graphique visuel des signaux bluetooth qui y sont décodés.

![system_image](Rapport/images_BE/set_up_system.jpeg)

#### Visualisation du graphe
Le graph visual des données des capteurs obtenu via bluetooth 

![pyqtgraph_image](Rapport/images_BE/visualisation.png)
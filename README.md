# steering-library

Une petite description du projet

:pushpin: ## Pour commencer

### Pré-requis

- SFML

### Installation

- Pour installer la librairie SFML sur Fedora faire les deux commandes suivantes :

sudo dnf -y install SFML
sudo dnf install SFML SFML-devel

- Mettre lien pour installer SFML sur Windows Linux et Mac

## Démarrage

- Pour compiler et lancer le projet, faire la commande suivante :

g++ steering_simulation.cpp -o steering_simulation -lsfml-graphics -lsfml-window -lsfml-system && ./steering_simulation

## Modes

- Chaque mode est associé à une touche :

* S : Seek
* F : Flee
* P : Pursuit
* E : Evasion
* A : Arrival
* C : Circuit
* O : One Way
* T : Two Way

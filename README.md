# 🐝 Steering Library

Une simulation de steering behaviors utilisant SFML pour animer une abeille qui réagit dynamiquement à différents modes de déplacement.

## 📌 Pour commencer

### 🔧 Pré-requis

Avant de commencer, assure-toi d'avoir SFML installé sur ton système !

### 💻 Installation

🔹 **Fedora**

Installe SFML avec la commande suivante :

```
sudo dnf -y install SFML
sudo dnf install SFML SFML-devel
```

🔹 **Autres systèmes d'exploitation**

- **Visual Studio (Windows) :** [Installation SFML sur Visual Studio](https://www.sfml-dev.org/tutorials/3.0/getting-started/visual-studio/)
- **Linux :** [Installation SFML sur Linux](https://www.sfml-dev.org/tutorials/3.0/getting-started/linux/)
- **macOS :** [Installation SFML sur macOS](https://www.sfml-dev.org/tutorials/3.0/getting-started/macos/)

## 🚀 Démarrage

Pour compiler et exécuter la simulation, exécute la commande suivante dans un terminal :

```
g++ steering_simulation.cpp -o steering_simulation -lsfml-graphics -lsfml-window -lsfml-system && ./steering_simulation
```

## 🎮 Modes de simulation

Chaque mode correspond à un comportement spécifique et est activé en appuyant sur la touche correspondante :

| Touche  | Mode | Descriprtion  |
|----------|----------|----------|
| S | Seek | L'abeille se déplace pour atteindre une cible fixe. |
| F | Flee | L'abeille s'éloigne d'une cible fixe. |
| P | Pursuit | L'abeille se déplace pour intercepter une cible en mouvement. |
| E | Evasion | L'abeille se déplace pour échapper à un poursuivant. |
| A | Arrival | L'abeille se déplace pour s'arrêter sur une cible fixe. |
| C | Circuit | L'abeille suit un chemin prédéfini en boucle. |
| O | One Way | L'abeille suit un chemin d'un point A à B. |
| T | Two Way | L'abeille suit un chemin aller-retour entre deux points. |

## 📷 Aperçu

![Mode Pursuit](https://github.com/user-attachments/assets/7058fd8f-3c71-4646-a0fd-119ef77f7ef8)

![Mode One Way](https://github.com/user-attachments/assets/4b0d0dec-d7e9-47c4-a1ba-e730ab0d52ce)


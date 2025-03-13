#include <cmath>
#include <iostream>
#include <vector>

using namespace sf;
using namespace std;

// Constante representant la vitesse de deplacement du joueur
const float PLAYER_MOVE_SPEED = 5.0f;

// Classe representant un joueur
class Player {

public:
    Vector2f position; // Position actuelle du joueur
    bool carryingFlower = false; // Indique si le joueur transporte une fleur

    // Constructeur initialisant un joueur avec sa position
    Player(Vector2f pos) 
        : position(pos) {}

    // Déplace le joueur vers le haut
    void up () {
        position.y -= PLAYER_MOVE_SPEED;
    }

    // Déplace le joueur vers le bas
    void down () {
        position.y += PLAYER_MOVE_SPEED;
    }

    // Déplace le joueur vers la droite
    void right () {
        position.x += PLAYER_MOVE_SPEED;
    }

    // Déplace le joueur vers la gauche
    void left () {
        position.x -= PLAYER_MOVE_SPEED;
    }

};
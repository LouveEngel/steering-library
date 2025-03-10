#include <cmath>
#include <iostream>
#include <vector>

using namespace sf;
using namespace std;

// Vitesse de déplacement du joueur
const float PLAYER_MOVE_SPEED = 5.0f;

class Player {

public:
    Vector2f position; // Position actuelle du joueur
    bool carryingFlower;
    int index;

    // Constructeur initialisant les proprietes du vehicule
    Player(Vector2f pos, bool carryingF, int i) 
        : position(pos), carryingFlower(carryingF), index(i) {}

    void up () {
        position.y -= PLAYER_MOVE_SPEED;
    }

    void down () {
        position.y += PLAYER_MOVE_SPEED;
    }

    void right () {
        position.x += PLAYER_MOVE_SPEED;
    }

    void left () {
        position.x -= PLAYER_MOVE_SPEED;
    }

};
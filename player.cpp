#include <cmath>
#include <iostream>
#include <vector>

using namespace sf;
using namespace std;

class Player {

public:
    Vector2f position; // Position actuelle du joueur
    bool carryingFlower;

    // Constructeur initialisant les proprietes du vehicule
    Player(Vector2f pos, bool carryingF) 
        : position(pos), carryingFlower(carryingF) {}

    void up () {
        position.y -= 5;
    }

    void down () {
        position.y += 5;
    }

    void right () {
        position.x += 5;
    }

    void left () {
        position.x -= 5;
    }

};
#include <SFML/Graphics.hpp> // Bibliotheque pour l'affichage graphique
#include <SFML/System.hpp>   // Bibliotheque pour les fonctionnalites systeme
#include <SFML/Window.hpp>   // Bibliotheque pour la gestion des evenements
#include <cmath>
#include <iostream>
#include <vector>
#include <map>
#include <cstdlib> // Pour rand()
#include <ctime> // Pour time()

#include "vehicle.h"
#include "player.cpp" // Inclusion du fichier contenant la classe Player
#include "vehicle.cpp" // Inclusion du fichier contenant la classe Vehicle

using namespace sf;
using namespace std;

constexpr int WIDTH = 800; // Largeur de la fenetre
constexpr int HEIGHT = 800; // Hauteur de la fenetre

// Distance d'arrêt pour le arrival
const float SLOWING_DISTANCE = 150.0f;

// Facteurs d'échelle pour les sprites
const float BEE_SCALE = 0.07f;
const float FLOWER_SCALE = 0.007f;
const float BEEHIVE_SCALE = 0.04f;

const sf::Color PATH_COLOR(255, 221, 161);

// Definition des points du chemin que le vehicule peut suivre
vector<Vector2f> path_points = {
    {140,40},{280,40},{440,40},{740,40},
    {40, 190}, {140,190},{440, 190},{590,190},{740, 190},
    {440,260},
    {140,310},{740,310},
    {140,440}, {320,440},{490,440},{620,440},{740,440},
    {140,540},{490,540},{740,540},
    {40,640},{140, 640},{340,640},{490, 640},{620,640},{740, 640},
    {40,750}, {340,750}, {740,750}
};

// Graphe représentant les connexions entre les points du chemin
map<int, vector<int>> graph = {
    {0, {1, 5}},
    {1, {0, 2}},
    {2, {1, 6}},
    {3, {8}},

    {4, {5}},
    {5, {0, 4, 10}},
    {6, {2, 7, 9}},
    {7,  {6, 8}},
    {8,  {3, 7, 11}},

    {9,  {6}},

    {10, {5, 12}},
    {11, {8, 16}},

    {12, {10, 13, 17}},
    {13, {12, 14}},
    {14, {13, 15, 18}},
    {15, {14, 16}},
    {16, {11, 15, 19}},

    {17, {12, 21}},
    {18, {14, 23}},
    {19, {16, 25}},

    {20, {21, 26}},
    {21, {17, 20, 22}},
    {22, {21, 23, 27}},
    {23, {18, 22, 24}},
    {24, {23, 25}},
    {25, {19, 24, 28}},

    {26, {20}},
    {27, {22}},
    {28, {25}}
};

// Definition des points ou peut se trouver la ruche
vector<Vector2f> beehive_points = {
    {590,90},{320,345}
};

int main() {
    // Creation de la fenetre SFML
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Planning Path Finding");
    window.setFramerateLimit(60); // Limite la frequence d'images a 60 FPS

    // Initialisation du générateur de nombres aléatoires
    srand(static_cast<unsigned int>(time(nullptr)));

    // Creation d'un vehicule avec position, vitesse et autres parametres
    Vehicle bee(10, {700, 700}, {1.0f, 1.0f}, 1.0f, 6.0f);

    // Creation du player
    Player player({50,200},false,-1);

    // Chargement des textures necessaires
    map<string, Texture> textures;
    loadTexture(textures, "bee", "Images/Bee.png");
    loadTexture(textures, "flower", "Images/Flower.png");
    loadTexture(textures, "beehive", "Images/Beehive.png");

    // Creation et configuration des sprites pour l'affichage
    Sprite bee_player(textures["bee"]);
    bee_player.setOrigin(bee_player.getTexture()->getSize().x / 2, bee_player.getTexture()->getSize().y / 2);
    bee_player.setScale(BEE_SCALE, BEE_SCALE);

    Sprite bee_ai(textures["bee"]);
    bee_ai.setOrigin(bee_ai.getTexture()->getSize().x / 2, bee_ai.getTexture()->getSize().y / 2);
    bee_ai.setScale(BEE_SCALE, BEE_SCALE);

    Sprite flower_sprite(textures["flower"]);
    flower_sprite.setOrigin(flower_sprite.getTexture()->getSize().x / 2, flower_sprite.getTexture()->getSize().y / 2);
    flower_sprite.setScale(FLOWER_SCALE, FLOWER_SCALE);

    sf::Sprite beehive_sprite(textures["beehive"]);
    beehive_sprite.setOrigin(beehive_sprite.getTexture()->getSize().x / 2, beehive_sprite.getTexture()->getSize().y / 2);
    beehive_sprite.setScale(BEEHIVE_SCALE, BEEHIVE_SCALE);

    int randomIndex = rand() % beehive_points.size();
    Vector2f selectedBeehive = beehive_points[randomIndex];
    beehive_sprite.setPosition(selectedBeehive);

    // Création du conteneur pour stocker les segments du chemin
    std::vector<RectangleShape> path;

    createRect(path, sf::Color::Green, 100, 190, selectedBeehive.x - 50, selectedBeehive.y - 50);

    // Création des segments pour le chemin
    createRect(path, PATH_COLOR, 80, 800, 700, 0);
    createRect(path, PATH_COLOR, 800, 80, 0, 600);
    createRect(path, PATH_COLOR, 80, 200, 0, 600);
    createRect(path, PATH_COLOR, 80, 200, 300, 600);
    createRect(path, PATH_COLOR, 80, 680, 100, 0);
    createRect(path, PATH_COLOR, 180, 80, 0, 150);
    createRect(path, PATH_COLOR, 380, 80, 100, 0);
    createRect(path, PATH_COLOR, 80, 300, 400, 0);
    createRect(path, PATH_COLOR, 380, 80, 400, 150);
    createRect(path, PATH_COLOR, 680, 80, 100, 400);
    createRect(path, PATH_COLOR, 80, 280, 450, 400);

    vector<Flower> flowers;
    for (size_t i = 0; i < path_points.size(); i++) {
        if (rand() % 2 == 0) {
            Flower f;
            f.sprite.setTexture(textures["flower"]);
            f.sprite.setOrigin(f.sprite.getTexture()->getSize().x / 2.f,
                               f.sprite.getTexture()->getSize().y / 2.f);
            f.sprite.setScale(0.007f, 0.007f);
            f.sprite.setPosition(path_points[i]);
            flowers.push_back(f);
        }
    } 

    // Boucle principale du programme
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close(); // Ferme la fenetre si l'utilisateur clique sur la croix
            }

            // Gestion des entrees clavier
            if (event.type == sf::Event::KeyPressed) {
                // Sauvegarde de la position actuelle
                sf::Vector2f oldPosition = player.position;

                if (event.key.code == sf::Keyboard::Z) {
                    player.up();
                    bee_player.setRotation(-90);
                }
                if (event.key.code == sf::Keyboard::Q) {
                    player.left();
                    bee_player.setRotation(-180);
                }
                if (event.key.code == sf::Keyboard::S) {
                    player.down();
                    bee_player.setRotation(90);
                }
                if (event.key.code == sf::Keyboard::D) {
                    player.right();
                    bee_player.setRotation(0);
                }
                
                bee_player.setPosition(player.position);

                FloatRect beeBounds = bee_player.getGlobalBounds();
                FloatRect segRuche = path[0].getGlobalBounds();
                bool estSurChemin = false;

                if (segRuche.contains(beeBounds.left, beeBounds.top) &&
                    segRuche.contains(beeBounds.left + beeBounds.width, beeBounds.top) &&
                    segRuche.contains(beeBounds.left, beeBounds.top + beeBounds.height) &&
                    segRuche.contains(beeBounds.left + beeBounds.width, beeBounds.top + beeBounds.height) ) {
                    estSurChemin = true;
                }
                else {
                    for (const auto& segment : path) {
                        FloatRect segBounds = segment.getGlobalBounds();
                        if (segBounds.contains(beeBounds.left, beeBounds.top) &&
                            segBounds.contains(beeBounds.left + beeBounds.width, beeBounds.top) &&
                            segBounds.contains(beeBounds.left, beeBounds.top + beeBounds.height) &&
                            segBounds.contains(beeBounds.left + beeBounds.width, beeBounds.top + beeBounds.height) ) {
                            estSurChemin = true;
                            break;
                        }
                    }
                }

                if (!estSurChemin) {
                    player.position = oldPosition;
                    bee_player.setPosition(oldPosition);
                }
            }

        }

        if(!player.carryingFlower){
            for(auto &f: flowers){
                if(!f.picked && !f.delivered){
                    if(bee_player.getGlobalBounds().intersects(f.sprite.getGlobalBounds())){
                        f.picked = true;
                        f.carrier = Carrier::Player;
                        player.carryingFlower = true;
                        break;
                    }
                }
            }
        }

        if(!bee.carryingFlower){
            for(auto &f: flowers){
                if(!f.picked && !f.delivered){
                    if(bee_ai.getGlobalBounds().intersects(f.sprite.getGlobalBounds())){
                        f.picked = true;
                        f.carrier = Carrier::AI;
                        bee.carryingFlower = true;
                        break;
                    }
                }
            }
        }

        for (auto& f : flowers) {
            if (f.picked && !f.delivered) {
                if (f.carrier == Carrier::Player)
                    f.sprite.setPosition(player.position + Vector2f(20, 0));
                else if (f.carrier == Carrier::AI)
                    f.sprite.setPosition(bee.position + Vector2f(20, 0));
            }
        }

        for (auto& f : flowers) {
            if (f.picked && !f.delivered) {
                if (f.carrier == Carrier::Player && bee_player.getGlobalBounds().intersects(beehive_sprite.getGlobalBounds())) {
                    f.delivered = true;
                    f.picked = false;
                    f.carrier = Carrier::None;
                    player.carryingFlower = false;
                } else if (f.carrier == Carrier::AI && bee_ai.getGlobalBounds().intersects(beehive_sprite.getGlobalBounds())) {
                    f.delivered = true;
                    f.picked = false;
                    f.carrier = Carrier::None;
                    bee.carryingFlower = false;
                    bee.bestIndex = -1;
                }
            }
        }

        // Mise à jour de la navigation de l'abeille IA via la méthode updateAI
        if (bee_ai.getGlobalBounds().intersects(bee_player.getGlobalBounds())) {
            bee.velocity = {0, 0};
            bee.position.x = bee.position.x + 20;
        }
        else {
        bee.updateAI(path_points, graph, flowers, selectedBeehive, SLOWING_DISTANCE);
        }

        // Efface la fenetre avec un fond vert
        window.clear(Color::Green);
        
        // Dessine tous les segments du chemin
        for (const auto& segment : path) {
            window.draw(segment);
        }

        window.draw(beehive_sprite);

        for (auto& f : flowers) {
            if (!f.delivered)
                window.draw(f.sprite);
        }

        bee_player.setPosition(player.position);
        window.draw(bee_player);

        // Mise a jour de la position et de la rotation de l'abeille (vehicule)
        bee_ai.setPosition(bee.position);
        if (bee.length(bee.velocity) > 0.1f) {
            float angle = atan2(bee.velocity.y, bee.velocity.x) * 180 / M_PI;
            bee_ai.setRotation(angle);
        }

        window.draw(bee_ai);

        window.display();
    }

    return 0;
}

// si plus de fleur, retourne à ruche et colision
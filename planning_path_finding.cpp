#include <SFML/Graphics.hpp> // Bibliotheque pour l'affichage graphique
#include <SFML/System.hpp>   // Bibliotheque pour les fonctionnalites systeme
#include <SFML/Window.hpp>   // Bibliotheque pour la gestion des evenements
#include <cmath>
#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>
#include <ctime>

#include "player.cpp" // Inclusion du fichier contenant la classe Player
#include "vehicle.cpp" // Inclusion du fichier contenant la classe Vehicle

using namespace sf;
using namespace std;

constexpr int WIDTH = 800; // Largeur de la fenetre
constexpr int HEIGHT = 800; // Hauteur de la fenetre

// Distance a laquelle un vehicule commence a ralentir lorsqu'il atteint son objectif
const float SLOWING_DISTANCE = 150.0f;

// Facteurs d'echelle pour les sprites (abeille, fleur, ruche)
const float BEE_SCALE = 0.07f;
const float FLOWER_SCALE = 0.007f;
const float BEEHIVE_SCALE = 0.04f;

// Couleur utilisee pour representer le chemin
const sf::Color PATH_COLOR(255, 221, 161);

// Definition des points de passage pour le chemin du vehicule
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

// Definition du graphe des connexions entre les points du chemin
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

// Definition des emplacements possibles pour la ruche
vector<Vector2f> beehive_points = {
    {590,90},{320,345}
};

int main() {
    // Creation de la fenetre SFML
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Planning Path Finding");
    window.setFramerateLimit(60); // Limite la frequence d'images a 60 FPS

    // Initialisation du generateur de nombres aleatoires
    srand(static_cast<unsigned int>(time(nullptr)));

    // Creation des entites du jeu : joueur et vehicule
    Vehicle bee(10, {700, 700}, {1.0f, 1.0f}, 1.0f, 6.0f);
    Player player({50,200});

    // Chargement des textures
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

    // Selection aleatoire d'une ruche
    int randomIndex = rand() % beehive_points.size();
    Vector2f selectedBeehive = beehive_points[randomIndex];
    beehive_sprite.setPosition(selectedBeehive);

    // Creation du conteneur pour stocker les segments du chemin
    std::vector<RectangleShape> path;

    // Creation des segments pour le chemin
    createRect(path, sf::Color::Green, 100, 190, selectedBeehive.x - 50, selectedBeehive.y - 50);
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

    // Placement aleatoire des fleurs sur le chemin
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

            // Gestion du deplacement du joueur
            if (event.type == sf::Event::KeyPressed) {
                // Sauvegarde de la position actuelle du joueur (au cas ou le mouvement est annule)
                sf::Vector2f oldPosition = player.position;

                // Deplacement du joueur et rotation du sprite du joueur en fonction de la touche pressee
                if (event.key.code == sf::Keyboard::Z) { // Haut
                    player.up();
                    bee_player.setRotation(-90);
                }
                if (event.key.code == sf::Keyboard::Q) { // Gauche
                    player.left();
                    bee_player.setRotation(-180);
                }
                if (event.key.code == sf::Keyboard::S) { // Bas
                    player.down();
                    bee_player.setRotation(90);
                }
                if (event.key.code == sf::Keyboard::D) { // Droite
                    player.right();
                    bee_player.setRotation(0);
                }
                
                // Mise a jour de la position du sprite du joueur
                bee_player.setPosition(player.position);

                // Verification que le joueur reste sur le chemin
                FloatRect beeBounds = bee_player.getGlobalBounds(); // Recupere la hitbox de l'abeille controlee par le joueur
                bool estSurChemin = false;

                // Verifie si les 4 coins du joueur sont contenus dans l'un des segments du chemin
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

                // Si le joueur sort du chemin, il est replace a son ancienne position
                if (!estSurChemin) {
                    player.position = oldPosition;
                    bee_player.setPosition(oldPosition);
                }
            }

        }

        // Verifie si le joueur ne transporte pas deja une fleur
        if (!player.carryingFlower) {
            for (auto &f : flowers) {
                // Si la fleur n'a pas ete ramassee ni livree
                if (!f.picked && !f.delivered) {
                    // Verifie si le joueur entre en collision avec la fleur
                    if (bee_player.getGlobalBounds().intersects(f.sprite.getGlobalBounds())) {
                        f.picked = true; // La fleur est ramassee
                        f.carrier = Carrier::Player; // Attribue la fleur au joueur
                        player.carryingFlower = true;
                        break; // Sort de la boucle une fois qu'une fleur est ramassee
                    }
                }
            }
        }

        // Verifie si le vehicule ne transporte pas deja une fleur
        if (!bee.carryingFlower) {
            for (auto &f : flowers) {
                // Si la fleur n'a pas ete ramassee ni livree
                if (!f.picked && !f.delivered) {
                    // Verifie si le vehicule entre en collision avec la fleur
                    if (bee_ai.getGlobalBounds().intersects(f.sprite.getGlobalBounds())) {
                        f.picked = true; // La fleur est ramassee
                        f.carrier = Carrier::AI; // Attribue la fleur a l'IA
                        bee.carryingFlower = true;
                        break; // Sort de la boucle une fois qu'une fleur est ramassee
                    }
                }
            }
        }

        // Mise a jour de la position de la fleur transportee
        for (auto& f : flowers) {
            // Si la fleur a ete ramassee mais qu'elle n'est pas livree
            if (f.picked && !f.delivered) {
                // Si le joueur transporte la fleur, elle suit le joueur
                if (f.carrier == Carrier::Player)
                    f.sprite.setPosition(player.position + Vector2f(20, 0));
                // Si l'IA transporte la fleur, elle suit l'IA
                else if (f.carrier == Carrier::AI)
                    f.sprite.setPosition(bee.position + Vector2f(20, 0));
            }
        }

        // Gestion des livraisons de fleurs a la ruche
        for (auto& f : flowers) {
            // Si la fleur a ete ramassee mais qu'elle n'est pas livree
            if (f.picked && !f.delivered) {
                // Verifie si le joueur atteint la ruche avec une fleur
                if (f.carrier == Carrier::Player && bee_player.getGlobalBounds().intersects(beehive_sprite.getGlobalBounds())) {
                    f.delivered = true; // Marque la fleur comme livree
                    f.picked = false; // Elle n'est plus transportee
                    f.carrier = Carrier::None; // Plus de porteur
                    player.carryingFlower = false; // Le joueur peut ramasser une autre fleur
                } 
                // Verifie si l'IA atteint la ruche avec une fleur
                else if (f.carrier == Carrier::AI && bee_ai.getGlobalBounds().intersects(beehive_sprite.getGlobalBounds())) {
                    f.delivered = true; // Marque la fleur comme livree
                    f.picked = false; // Elle n'est plus transportee
                    f.carrier = Carrier::None; // Plus de porteur
                    bee.carryingFlower = false; // L'IA peut ramasser une autre fleur
                    bee.bestIndex = -1; // Reinitialise la cible de l'IA
                }
            }
        }

        // Verifie s'il y a une collision entre le joueur et l'IA
        if (bee_ai.getGlobalBounds().intersects(bee_player.getGlobalBounds())) {
            // Arreter le vehicule en cas de collision avec le joueur
            bee.velocity = {0, 0};

            // Calculer la difference de position entre le vehicule et le player
            Vector2f delta = bee.position - player.position;

            // On determine quelle composante (x ou y) est dominante
            if (std::abs(delta.x) > std::abs(delta.y)) {
                // Collision principalement horizontale
                if (delta.x < 0) {
                    // Le vehicule vient de la gauche -> reculer a gauche (diminue x)
                    bee.position.x -= 20;
                } else {
                    // Le vehicule vient de la droite -> reculer a droite (augmente x)
                    bee.position.x += 20;
                }
            } else {
                // Collision principalement verticale
                if (delta.y < 0) {
                    // Le vehicule vient du haut -> reculer vers le haut (diminue y)
                    bee.position.y -= 20;
                } else {
                    // Le vehicule vient du bas -> reculer vers le bas (augmente y)
                    bee.position.y += 20;
                }
            }
        } else {
            // Aucune collision : mise a jour du vehicule
            bee.updateAI(path_points, graph, flowers, selectedBeehive, SLOWING_DISTANCE);
        }

        // Efface la fenetre avec un fond vert
        window.clear(Color::Green);
        
        // Dessine tous les segments du chemin
        for (const auto& segment : path) {
            window.draw(segment);
        }

        // Dessine la ruche
        window.draw(beehive_sprite);

        // Dessine les fleurs qui n'ont pas encore ete livrees
        for (auto& f : flowers) {
            if (!f.delivered)
                window.draw(f.sprite);
        }

        // Dessine le joueur
        bee_player.setPosition(player.position);
        window.draw(bee_player);

        // Mise a jour de la position et de la rotation du vehicule
        bee_ai.setPosition(bee.position);
        if (bee.length(bee.velocity) > 0.1f) {
            float angle = atan2(bee.velocity.y, bee.velocity.x) * 180 / M_PI;
            bee_ai.setRotation(angle);
        }

        // Dessine le vehicule
        window.draw(bee_ai);

        window.display();
    }

    return 0;
}
#include <SFML/Graphics.hpp> // Bibliotheque pour l'affichage graphique
#include <SFML/System.hpp>   // Bibliotheque pour les fonctionnalites systeme
#include <SFML/Window.hpp>   // Bibliotheque pour la gestion des evenements
#include <cmath>
#include <iostream>
#include <vector>
#include <map>

#include "player.cpp"
#include "vehicle.cpp" // Inclusion du fichier contenant la classe Vehicle

using namespace sf;
using namespace std;

constexpr int WIDTH = 800; // Largeur de la fenetre
constexpr int HEIGHT = 800; // Hauteur de la fenetre

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

enum class BeeState {
    GoToFlower,
    ReturnToHive,
};

// Cette fonction permet de charger une texture a partir d'un fichier
void loadTexture(map<string, Texture>& textures, const string& name, const string& path) {
    if (!textures[name].loadFromFile(path)) {
        cerr << "Erreur : Impossible de charger " << path << endl;
        exit(-1);
    }
}

int main() {
    // Creation de la fenetre SFML
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Planning Path Finding");
    window.setFramerateLimit(60); // Limite la frequence d'images a 60 FPS

    // Creation d'un vehicule avec position, vitesse et autres parametres
    Vehicle bee(10, {100, 100}, {1.0f, 1.0f}, 1.0f, 5.0f);

    // Chargement des textures necessaires
    map<string, Texture> textures;
    loadTexture(textures, "bee", "Images/Bee.png");
    loadTexture(textures, "flower", "Images/Flower.png");
    loadTexture(textures, "beehive", "Images/Beehive.png");

    Player player({50,200});

    // Creation et configuration des sprites pour l'affichage
    Sprite bee_sprite(textures["bee"]);
    bee_sprite.setOrigin(bee_sprite.getTexture()->getSize().x / 2, bee_sprite.getTexture()->getSize().y / 2);
    bee_sprite.setScale(0.07f, 0.07f);

    Sprite bee_sprite2(textures["bee"]);
    bee_sprite2.setOrigin(bee_sprite2.getTexture()->getSize().x / 2, bee_sprite2.getTexture()->getSize().y / 2);
    bee_sprite2.setScale(0.07f, 0.07f);

    Sprite flower_sprite(textures["flower"]);
    flower_sprite.setOrigin(flower_sprite.getTexture()->getSize().x / 2, flower_sprite.getTexture()->getSize().y / 2);
    flower_sprite.setScale(0.007f, 0.007f);
    Vector2f flowerPos(50, 50);
    flower_sprite.setPosition(flowerPos);

    sf::Sprite beehive_sprite(textures["beehive"]);
    beehive_sprite.setOrigin(beehive_sprite.getTexture()->getSize().x / 2, beehive_sprite.getTexture()->getSize().y / 2);
    beehive_sprite.setScale(0.07f, 0.07f);
    Vector2f hivePos(750, 500);
    beehive_sprite.setPosition(hivePos);


    sf::Color color(255, 221, 161);

    // Création du conteneur pour stocker les segments du chemin
    std::vector<RectangleShape> chemin;

    // Segment 1
    RectangleShape rect1;
    rect1.setSize(Vector2f(80, 800));
    rect1.setFillColor(Color(255, 221, 161));
    rect1.setPosition(700, 0);
    chemin.push_back(rect1);

    // Segment 2
    RectangleShape rect2;
    rect2.setSize(Vector2f(800, 80));
    rect2.setFillColor(Color(255, 221, 161));
    rect2.setPosition(0, 600);
    chemin.push_back(rect2);

    // Segment 3
    RectangleShape rect3;
    rect3.setSize(Vector2f(80, 200));
    rect3.setFillColor(Color(255, 221, 161));
    rect3.setPosition(0, 600);
    chemin.push_back(rect3);

    // Segment 4
    RectangleShape rect4;
    rect4.setSize(Vector2f(80, 200));
    rect4.setFillColor(Color(255, 221, 161));
    rect4.setPosition(300, 600);
    chemin.push_back(rect4);

    // Segment 5
    RectangleShape rect5;
    rect5.setSize(Vector2f(80, 680));
    rect5.setFillColor(Color(255, 221, 161));
    rect5.setPosition(100, 0);
    chemin.push_back(rect5);

    // Segment 6
    RectangleShape rect6;
    rect6.setSize(Vector2f(180, 80));
    rect6.setFillColor(Color(255, 221, 161));
    rect6.setPosition(0, 150);
    chemin.push_back(rect6);

    // Segment 7
    RectangleShape rect7;
    rect7.setSize(Vector2f(380, 80));
    rect7.setFillColor(Color(255, 221, 161));
    rect7.setPosition(100, 0);
    chemin.push_back(rect7);

    // Segment 8
    RectangleShape rect8;
    rect8.setSize(Vector2f(80, 300));
    rect8.setFillColor(Color(255, 221, 161));
    rect8.setPosition(400, 0);
    chemin.push_back(rect8);

    // Segment 9
    RectangleShape rect9;
    rect9.setSize(Vector2f(380, 80));
    rect9.setFillColor(Color(255, 221, 161));
    rect9.setPosition(400, 150);
    chemin.push_back(rect9);

    // Segment 10
    RectangleShape rect10;
    rect10.setSize(Vector2f(680, 80));
    rect10.setFillColor(Color(255, 221, 161));
    rect10.setPosition(100, 400);
    chemin.push_back(rect10);

    // Segment 11
    RectangleShape rect11;
    rect11.setSize(Vector2f(80, 280));
    rect11.setFillColor(Color(255, 221, 161));
    rect11.setPosition(450, 400);
    chemin.push_back(rect11);

    BeeState beeState = BeeState::GoToFlower;
    float slowingDistance = 200.0f;
    

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
                    bee_sprite.setRotation(-90);
                }
                if (event.key.code == sf::Keyboard::Q) {
                    player.left();
                    bee_sprite.setRotation(-180);
                }
                if (event.key.code == sf::Keyboard::S) {
                    player.down();
                    bee_sprite.setRotation(90);
                }
                if (event.key.code == sf::Keyboard::D) {
                    player.right();
                    bee_sprite.setRotation(0);
                }
                
                bee_sprite.setPosition(player.position);

                FloatRect beeBounds = bee_sprite.getGlobalBounds();
                bool estSurChemin = false;

                for (const auto& segment : chemin) {
                    FloatRect segBounds = segment.getGlobalBounds();
                    if ( segBounds.contains(beeBounds.left, beeBounds.top) &&
                        segBounds.contains(beeBounds.left + beeBounds.width, beeBounds.top) &&
                        segBounds.contains(beeBounds.left, beeBounds.top + beeBounds.height) &&
                        segBounds.contains(beeBounds.left + beeBounds.width, beeBounds.top + beeBounds.height) ) {
                        estSurChemin = true;
                        break;
                    }
                }

                if (!estSurChemin) {
                    player.position = oldPosition;
                    bee_sprite.setPosition(oldPosition);
                }
            }

        }

        if (beeState == BeeState::GoToFlower) {
            bee.seek(false, (flowerPos));
            Vector2f diff = flowerPos - bee.position;
            float d = sqrt(diff.x * diff.x + diff.y * diff.y);
            if (d < 10.0f) {
                beeState = BeeState::ReturnToHive;
            }
        }
        else if (beeState == BeeState::ReturnToHive) {
            bee.arrival(hivePos, slowingDistance);
            Vector2f diff = hivePos - bee.position;
            float d = sqrt(diff.x * diff.x + diff.y * diff.y);
        }

        // Mise a jour de la position et de la rotation de l'abeille (vehicule)
        bee_sprite2.setPosition(bee.position);
        if (bee.length(bee.velocity) > 0.1f) {
            float angle = atan2(bee.velocity.y, bee.velocity.x) * 180 / M_PI;
            bee_sprite2.setRotation(angle);
        }

        // Efface la fenetre avec un fond vert
        window.clear(Color::Green);
        
        // Dessine tous les segments du chemin
        for (const auto& segment : chemin) {
            window.draw(segment);
        }

        if (beeState == BeeState::GoToFlower)
            window.draw(flower_sprite);

        window.draw(beehive_sprite);
        
        window.draw(bee_sprite2);
        
        bee_sprite.setPosition(player.position);
        window.draw(bee_sprite);

        // Placement des objets (fleurs, ruche, lac) aux points du chemin
        for (size_t i = 0; i < path_points.size(); i++) {
            flower_sprite.setPosition(path_points[i]); // Place une fleur
            window.draw(flower_sprite); // Affiche la fleur
        }

        window.display();
    }

    return 0;
}
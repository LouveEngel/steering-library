#include <SFML/Graphics.hpp> // Bibliotheque pour l'affichage graphique
#include <SFML/System.hpp>   // Bibliotheque pour les fonctionnalites systeme
#include <SFML/Window.hpp>   // Bibliotheque pour la gestion des evenements
#include <cmath>
#include <iostream>
#include <vector>
#include <map>
#include <cstdlib>    // Pour rand()
#include <ctime>      // Pour time()

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

// Definition des points ou peut se trouver la ruche
vector<Vector2f> hive_points = {
    /*{50,90},*/{590,90},{320,345}/*,{50,540},{640,740}*/
};

struct Flower {
    Sprite sprite;
    bool picked = false;
    bool delivered = false;
};

// Cette fonction permet de charger une texture a partir d'un fichier
void loadTexture(map<string, Texture>& textures, const string& name, const string& path) {
    if (!textures[name].loadFromFile(path)) {
        cerr << "Erreur : Impossible de charger " << path << endl;
        exit(-1);
    }
}

void createRect(std::vector<RectangleShape>& chemin, int sizeX, int sizeY, int positionX, int positionY) {
    RectangleShape rect;
    rect.setSize(Vector2f(sizeX, sizeY));
    rect.setFillColor(Color(255, 221, 161));
    rect.setPosition(positionX, positionY);
    chemin.push_back(rect);
}

int main() {
    // Creation de la fenetre SFML
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Planning Path Finding");
    window.setFramerateLimit(60); // Limite la frequence d'images a 60 FPS

    // Initialisation du générateur de nombres aléatoires
    srand(static_cast<unsigned int>(time(nullptr)));

    // Creation d'un vehicule avec position, vitesse et autres parametres
    Vehicle bee(10, {700, 700}, {1.0f, 1.0f}, 1.0f, 5.0f, -1);

    Player player({50,200},false);

    // Chargement des textures necessaires
    map<string, Texture> textures;
    loadTexture(textures, "bee", "Images/Bee.png");
    loadTexture(textures, "flower", "Images/Flower.png");
    loadTexture(textures, "beehive", "Images/Beehive.png");

    // Creation et configuration des sprites pour l'affichage
    Sprite bee_player(textures["bee"]);
    bee_player.setOrigin(bee_player.getTexture()->getSize().x / 2, bee_player.getTexture()->getSize().y / 2);
    bee_player.setScale(0.07f, 0.07f);

    Sprite bee_ai(textures["bee"]);
    bee_ai.setOrigin(bee_ai.getTexture()->getSize().x / 2, bee_ai.getTexture()->getSize().y / 2);
    bee_ai.setScale(0.07f, 0.07f);

    Sprite flower_sprite(textures["flower"]);
    flower_sprite.setOrigin(flower_sprite.getTexture()->getSize().x / 2, flower_sprite.getTexture()->getSize().y / 2);
    flower_sprite.setScale(0.007f, 0.007f);

    sf::Sprite beehive_sprite(textures["beehive"]);
    beehive_sprite.setOrigin(beehive_sprite.getTexture()->getSize().x / 2, beehive_sprite.getTexture()->getSize().y / 2);
    beehive_sprite.setScale(0.04f, 0.04f);

    sf::Color color(255, 221, 161);

    // Création du conteneur pour stocker les segments du chemin
    std::vector<RectangleShape> chemin;

    RectangleShape rectRuche;
    rectRuche.setSize(Vector2f(100, 190));
    rectRuche.setFillColor(sf::Color::Red);
    
    int randomIndex = rand() % hive_points.size();
    Vector2f selectedHive = hive_points[randomIndex];
    beehive_sprite.setPosition(selectedHive);

    rectRuche.setPosition(
        selectedHive.x - 50,
        selectedHive.y - 50
    );

    chemin.push_back(rectRuche);

    // Segment 1
    createRect(chemin, 80, 800, 700, 0);

    // Segment 2
    createRect(chemin, 800, 80, 0, 600);

    // Segment 3
    createRect(chemin, 80, 200, 0, 600);

    // Segment 4
    createRect(chemin, 80, 200, 300, 600);

    // Segment 5
    createRect(chemin, 80, 680, 100, 0);

    // Segment 6
    createRect(chemin, 180, 80, 0, 150);

    // Segment 7
    createRect(chemin, 380, 80, 100, 0);

    // Segment 8
    createRect(chemin, 80, 300, 400, 0);

    // Segment 9
    createRect(chemin, 380, 80, 400, 150);

    // Segment 10
    createRect(chemin, 680, 80, 100, 400);

    // Segment 11
    createRect(chemin, 80, 280, 450, 400);

    float slowingDistance = 200.0f;

    vector<Vector2f> flowerPositions;
    vector<Flower> flowers;
    for (size_t i = 0; i < path_points.size(); i++) {
        if (rand() % 2 == 0) {
            flowerPositions.push_back(path_points[i]);
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
                FloatRect segRuche = rectRuche.getGlobalBounds();
                bool estSurChemin = false;

                if ( segRuche.contains(beeBounds.left, beeBounds.top) &&
                    segRuche.contains(beeBounds.left + beeBounds.width, beeBounds.top) &&
                    segRuche.contains(beeBounds.left, beeBounds.top + beeBounds.height) &&
                    segRuche.contains(beeBounds.left + beeBounds.width, beeBounds.top + beeBounds.height) ) {
                    estSurChemin = true;
                }
                else {
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
                }

                if (!estSurChemin) {
                    player.position = oldPosition;
                    bee_player.setPosition(oldPosition);
                }
            }

        }

        if(!player.carryingFlower) {
            for (auto& f : flowers) {
                if (!f.picked && !f.delivered) {
                    if (bee_player.getGlobalBounds().intersects(f.sprite.getGlobalBounds())) {
                        f.picked = true;
                        player.carryingFlower = true;
                    }
                }
            }
        }

        for (auto& f : flowers) {
            if (f.picked && !f.delivered) {
                f.sprite.setPosition(player.position + Vector2f(20, 0));
            }
        }

        for (auto& f : flowers) {
            if (f.picked && !f.delivered) {
                if (bee_player.getGlobalBounds().intersects(beehive_sprite.getGlobalBounds())) {
                    f.delivered = true;
                    f.picked = false;
                    player.carryingFlower = false;
                }
            }
        }

        if (bee.bestIndex == -1) {
            bee.findClosestPointIndex(flowerPositions);
        }
        bee.seek(false, flowerPositions[bee.bestIndex]);

        // Efface la fenetre avec un fond vert
        window.clear(Color::Green);
        
        // Dessine tous les segments du chemin
        for (const auto& segment : chemin) {
            window.draw(segment);
        }

        window.draw(beehive_sprite);

        for (auto& f : flowers) {
            if (!f.delivered)
                window.draw(f.sprite);
        }

        bee_player.setPosition(player.position);
        window.draw(bee_player);

        bee_ai.setPosition(bee.position);
        window.draw(bee_ai);

        window.display();
    }

    return 0;
}
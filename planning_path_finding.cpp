#include <SFML/Graphics.hpp> // Bibliotheque pour l'affichage graphique
#include <SFML/System.hpp>   // Bibliotheque pour les fonctionnalites systeme
#include <SFML/Window.hpp>   // Bibliotheque pour la gestion des evenements
#include <cmath>
#include <iostream>
#include <vector>
#include <map>

#include "player.cpp"

using namespace sf;
using namespace std;

constexpr int WIDTH = 800; // Largeur de la fenetre
constexpr int HEIGHT = 800; // Hauteur de la fenetre

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

    // Chargement des textures necessaires
    map<string, Texture> textures;
    loadTexture(textures, "bee", "Images/Bee.png");

    Player player({50,80});

    // Creation et configuration des sprites pour l'affichage
    Sprite bee_sprite(textures["bee"]);
    bee_sprite.setOrigin(bee_sprite.getTexture()->getSize().x / 2, bee_sprite.getTexture()->getSize().y / 2);
    bee_sprite.setScale(0.07f, 0.07f);

    sf::Color color(255, 221, 161);

    sf::RectangleShape rectangle1;
    rectangle1.setSize(sf::Vector2f(700, 60));
    rectangle1.setFillColor(color);
    rectangle1.setPosition(0, 50);

    sf::RectangleShape rectangle2;
    rectangle2.setSize(sf::Vector2f(60, 600));
    rectangle2.setFillColor(color);
    rectangle2.setPosition(700, 50);

    sf::RectangleShape rectangle3;
    rectangle3.setSize(sf::Vector2f(400, 60));
    rectangle3.setFillColor(color);
    rectangle3.setPosition(100, 700);

    sf::CircleShape circle;
    circle.setRadius(10);
    circle.setFillColor(sf::Color::Red);
    
    

    // Boucle principale du programme
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close(); // Ferme la fenetre si l'utilisateur clique sur la croix
            }

            // Gestion des entrees clavier
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::Z) {
                    player.up();
                    bee_sprite.setRotation(-90);
                }
                if (event.key.code == sf::Keyboard::Q) {
                    player.left();
                    bee_sprite.setRotation(-180);
                    circle.setPosition(player.position.x - 30, player.position.y - 10);
                }
                if (event.key.code == sf::Keyboard::S) {
                    player.down();
                    bee_sprite.setRotation(90);
                }
                if (event.key.code == sf::Keyboard::D) {
                    player.right();
                    bee_sprite.setRotation(0);
                    circle.setPosition(player.position.x + 10, player.position.y);
                }
            }
        }

        // Efface la fenetre avec un fond vert
        window.clear(Color::Green);
        
        window.draw(rectangle1);
        window.draw(rectangle2);
        window.draw(rectangle3);
        
        

        bee_sprite.setPosition(player.position);
        window.draw(bee_sprite);

        window.draw(circle);

        window.display();
    }

    return 0;
}
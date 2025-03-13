#include <SFML/Graphics.hpp> // Bibliotheque pour l'affichage graphique
#include <SFML/System.hpp>   // Bibliotheque pour les fonctionnalites systeme
#include <SFML/Window.hpp>   // Bibliotheque pour la gestion des evenements
#include <cmath>
#include <iostream>
#include <vector>
#include <map>

#include "vehicle.cpp" // Inclusion du fichier contenant la classe Vehicle

using namespace sf;
using namespace std;

constexpr int WIDTH = 800; // Largeur de la fenetre
constexpr int HEIGHT = 800; // Hauteur de la fenetre

// Definition des points du chemin que le vehicule peut suivre
vector<Vector2f> path_points = {
    {150, 200}, {400, 200}, {650, 200}, {650, 600}, {400, 600}, {150, 600}
};

// Fonction qui charge une texture depuis un fichier et l'associe a un nom dans une map
void loadTexture(map<string, Texture>& textures, const string& name, const string& path) {
    // Tente de charger la texture a partir du chemin specifie
    if (!textures[name].loadFromFile(path)) {
        // En cas d'echec, affiche un message d'erreur et termine le programme
        cerr << "Erreur : Impossible de charger " << path << endl;
        exit(-1);
    }
}

int main() {
    // Creation de la fenetre SFML
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Steering Simulation");
    window.setFramerateLimit(60); // Limite la frequence d'images a 60 FPS

    // Creation d'un vehicule avec position, vitesse et autres parametres
    Vehicle vehicle(10, {100, 100}, {1.0f, 1.0f}, 1.0f, 5.0f);
    string mode = "Seek"; // Mode par defaut du vehicule
    int current_target_index = 0; // Index du point cible sur le chemin

    // Variables pour gerer les deplacements et les comportements du vehicule
    bool inverse = false;
    float arrival_timer = 2.0f;
    float slowing_distance = 200.0f;

    // Chargement des textures necessaires
    map<string, Texture> textures;
    loadTexture(textures, "bee", "Images/Bee.png");
    loadTexture(textures, "flower", "Images/Flower.png");
    loadTexture(textures, "bird", "Images/Bird.png");
    loadTexture(textures, "beehive", "Images/Beehive.png");
    loadTexture(textures, "lake", "Images/Lake.png");

    // Creation et configuration des sprites pour l'affichage
    Sprite bee_sprite(textures["bee"]);
    bee_sprite.setOrigin(bee_sprite.getTexture()->getSize().x / 2, bee_sprite.getTexture()->getSize().y / 2);
    bee_sprite.setScale(0.1f, 0.1f);

    Sprite flower_sprite(textures["flower"]);
    flower_sprite.setOrigin(flower_sprite.getTexture()->getSize().x / 2, flower_sprite.getTexture()->getSize().y / 2);
    flower_sprite.setScale(0.02f, 0.02f);

    sf::Sprite bird_sprite(textures["bird"]);
    bird_sprite.setOrigin(bird_sprite.getTexture()->getSize().x / 2, bird_sprite.getTexture()->getSize().y / 2);
    bird_sprite.setScale(0.1f, 0.1f);

    sf::Sprite beehive_sprite(textures["beehive"]);
    beehive_sprite.setOrigin(beehive_sprite.getTexture()->getSize().x / 2, beehive_sprite.getTexture()->getSize().y / 2);
    beehive_sprite.setScale(0.1f, 0.1f);

    sf::Sprite lake_sprite(textures["lake"]);
    lake_sprite.setOrigin(lake_sprite.getTexture()->getSize().x / 2, lake_sprite.getTexture()->getSize().y / 2);
    lake_sprite.setScale(0.4f, 0.4f);

    // Chargement de la police et creation du texte d'affichage du mode actuel
    sf::Font font;
    if (!font.loadFromFile("Typeface/Salsa-Regular.ttf")) {
        cerr << "Erreur : Impossible de charger la police arial.ttf" << endl;
        return -1;
    }

    sf::Text modeText;
    modeText.setFont(font);
    modeText.setCharacterSize(27);
    modeText.setFillColor(sf::Color::Black);
    modeText.setPosition(300, 21);
    modeText.setString("Mode: " + mode);

    // Boucle principale du programme
    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close(); // Ferme la fenetre si l'utilisateur clique sur la croix
            }

            // Gestion des entrees clavier pour changer le mode du vehicule
            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::S) mode = "Seek";
                if (event.key.code == sf::Keyboard::F) mode = "Flee";
                if (event.key.code == sf::Keyboard::P) mode = "Pursuit";
                if (event.key.code == sf::Keyboard::E) mode = "Evasion";
                if (event.key.code == sf::Keyboard::A) mode = "Arrival";
                if (event.key.code == sf::Keyboard::C) mode = "Circuit";
                if (event.key.code == sf::Keyboard::O) mode = "One Way";
                if (event.key.code == sf::Keyboard::T) mode = "Two Way";
                modeText.setString("Mode: " + mode);
            }
        }

        // Detection de la position de la souris comme cible pour le vehicule
        Vector2f target_pos = Vector2f(Mouse::getPosition(window));
        if (mode == "Flee" || mode == "Evasion") {
            bird_sprite.setPosition(target_pos);
        } else {
            flower_sprite.setPosition(target_pos);
        }

        // Application des comportements selon le mode choisi
        if (mode == "Seek") vehicle.seek(false, target_pos);
        else if (mode == "Flee") vehicle.seek(true, target_pos);
        else if (mode == "Pursuit") vehicle.pursuit(false, target_pos);
        else if (mode == "Evasion") vehicle.pursuit(true, target_pos);
        else if (mode == "Arrival") vehicle.arrival(target_pos, slowing_distance);
        else if (mode == "Circuit") vehicle.circuit(current_target_index, path_points);
        else if (mode == "One Way") vehicle.one_way(current_target_index, path_points, slowing_distance);
        else if (mode == "Two Way") vehicle.two_way(current_target_index, inverse, arrival_timer, path_points, slowing_distance);

        // Efface la fenetre avec un fond vert
        window.clear(Color::Green);

        // Verifie si le mode selectionne correspond a un chemin predefini
        if (mode == "One Way" || mode == "Two Way" || mode == "Circuit") { 
            // Dessine les lignes du chemin en reliant les points stockes dans path_points
            for (size_t i = 0; i < path_points.size(); i++) {
                // Condition pour ne pas tracer la dernieere ligne dans certains modes
                if (!((mode == "One Way" && i == path_points.size() - 1) || 
                    (mode == "Two Way" && i == path_points.size() - 1))) {
                    // Creation d'une ligne blanche entre le point actuel et le suivant
                    Vertex line[] = {
                        Vertex(path_points[i], Color::White),
                        Vertex(path_points[(i + 1) % path_points.size()], Color::White)
                    };
                    window.draw(line, 2, Lines); // Dessine la ligne
                }
            }

            // Placement des objets (fleurs, ruche, lac) aux points du chemin
            for (size_t i = 0; i < path_points.size(); i++) {
                // Si le point actuel est le dernier du chemin dans les modes One Way ou Two Way
                if ((mode == "One Way" && i == path_points.size() - 1) || 
                    (mode == "Two Way" && i == path_points.size() - 1)) {
                    beehive_sprite.setPosition(path_points[i]); // Place une ruche
                    window.draw(beehive_sprite); // Affiche la ruche
                } 
                // Si c'est le premier point du chemin en mode Two Way
                else if (mode == "Two Way" && i == 0) {
                    lake_sprite.setPosition(path_points[i]); // Place un lac
                    window.draw(lake_sprite); // Affiche le lac
                } 
                // Tous les autres points ont une fleur
                else {
                    flower_sprite.setPosition(path_points[i]); // Place une fleur
                    window.draw(flower_sprite); // Affiche la fleur
                }
            }
        // Si le mode selectionne n'est pas un mode de parcours (One Way, Two Way, Circuit)
        } else {
            // Si le mode est Flee (fuite) ou Evasion, on affiche un oiseau
            if (mode == "Flee" || mode == "Evasion") {
                window.draw(bird_sprite);
            // Sinon, on affiche une fleur comme cible
            } else {
                window.draw(flower_sprite);
            }
        }

        // Mise a jour de la position et de la rotation de l'abeille (vehicule)
        bee_sprite.setPosition(vehicle.position);
        if (vehicle.length(vehicle.velocity) > 0.1f) {
            float angle = atan2(vehicle.velocity.y, vehicle.velocity.x) * 180 / M_PI;
            bee_sprite.setRotation(angle);
        }
        
        // Affichage de l'abeille (vehicule) et du texte a l'ecran
        window.draw(bee_sprite);
        window.draw(modeText);
        window.display();
    }

    return 0;
}
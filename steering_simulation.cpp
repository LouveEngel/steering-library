#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <iostream>
#include <vector>
#include <map>

#include "vehicle.cpp"

using namespace sf;
using namespace std;

constexpr int WIDTH = 800;
constexpr int HEIGHT = 800;

vector<Vector2f> path_points = {
    {150, 200}, {400, 200}, {650, 200}, {650, 600}, {400, 600}, {150, 600}
};

void loadTexture(map<string, Texture>& textures, const string& name, const string& path) {
    if (!textures[name].loadFromFile(path)) {
        cerr << "Erreur : Impossible de charger " << path << endl;
        exit(-1);
    }
}

int main() {
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Steering Simulation");
    window.setFramerateLimit(60);

    Vehicle vehicle(10, {100, 100}, {1.0f, 1.0f}, 1.0f, 5.0f);
    string mode = "Seek";
    int current_target_index = 0;

    bool inverse = false;
    float arrival_timer = 2.0f;
    float slowing_distance = 200.0f;

    map<string, Texture> textures;
    loadTexture(textures, "bee", "Bee.png");
    loadTexture(textures, "flower", "Flower.png");
    loadTexture(textures, "bird", "Bird.png");
    loadTexture(textures, "beehive", "Beehive.png");
    loadTexture(textures, "lake", "Lake.png");

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


    sf::Font font;
    if (!font.loadFromFile("Salsa-Regular.ttf")) {
        cerr << "Erreur : Impossible de charger la police arial.ttf" << endl;
        return -1;
    }

    sf::Text modeText;
    modeText.setFont(font);
    modeText.setCharacterSize(27);
    modeText.setFillColor(sf::Color::Black);
    modeText.setPosition(300, 21);
    modeText.setString("Mode: " + mode);


    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

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

        Vector2f target_pos = Vector2f(Mouse::getPosition(window));
        if (mode == "Flee" || mode == "Evasion") {
            bird_sprite.setPosition(target_pos);
        } else {
            flower_sprite.setPosition(target_pos);
        }

        if (mode == "Seek") vehicle.seek(false, target_pos);
        else if (mode == "Flee") vehicle.seek(true, target_pos);
        else if (mode == "Pursuit") vehicle.pursuit(false, target_pos);
        else if (mode == "Evasion") vehicle.pursuit(true, target_pos);
        else if (mode == "Arrival") vehicle.arrival(target_pos, slowing_distance);
        else if (mode == "Circuit") vehicle.circuit(current_target_index, path_points);
        else if (mode == "One Way") vehicle.one_way(current_target_index, path_points, slowing_distance);
        else if (mode == "Two Way") vehicle.two_way(current_target_index, inverse, arrival_timer, path_points, slowing_distance);

        window.clear(Color::Green);

        if (mode == "One Way" || mode == "Two Way" || mode == "Circuit") {

            for (size_t i = 0; i < path_points.size(); i++) {
                if (!((mode == "One Way" && i == path_points.size() - 1) || (mode == "Two Way" && i == path_points.size() - 1))) {
                    Vertex line[] = {
                        Vertex(path_points[i], Color::White),
                        Vertex(path_points[(i + 1) % path_points.size()], Color::White)
                    };
                    window.draw(line, 2, Lines);
                }
            }

            for (size_t i = 0; i < path_points.size(); i++) {
                if ((mode == "One Way" && i == path_points.size() - 1) || (mode == "Two Way" && i == path_points.size() - 1)) {
                    beehive_sprite.setPosition(path_points[i]);
                    window.draw(beehive_sprite);
                } else if (mode == "Two Way" && i == 0) {
                    lake_sprite.setPosition(path_points[i]);
                    window.draw(lake_sprite);
                } else {
                    Sprite flowerOnPath(textures["flower"]);
                    flowerOnPath.setOrigin(textures["flower"].getSize().x / 2, textures["flower"].getSize().y / 2);
                    flowerOnPath.setScale(0.02f, 0.02f);
                    flowerOnPath.setPosition(path_points[i]);
                    window.draw(flowerOnPath);
                }
            }
        } else {
            if (mode == "Flee" || mode == "Evasion") {
                window.draw(bird_sprite);
            } else {
                window.draw(flower_sprite);
            }
        }

        bee_sprite.setPosition(vehicle.position);
        if (vehicle.length(vehicle.velocity) > 0.1f) {
            float angle = atan2(vehicle.velocity.y, vehicle.velocity.x) * 180 / M_PI;
            bee_sprite.setRotation(angle);
        }
        
        window.draw(bee_sprite);
        window.draw(modeText);
        window.display();
    }

    return 0;
}
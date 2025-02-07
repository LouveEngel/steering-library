#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <iostream>
#include <vector>

using namespace sf;
using namespace std;

const int WIDTH = 800;
const int HEIGHT = 800;
const float slowing_distance = 200.0f;

std::vector<Vector2f> path_points = {
    {150, 200}, {400, 200}, {650, 200}, {650, 600}, {400, 600}, {150, 600}
};

class Vehicle {
public:
    float mass;
    Vector2f position;
    Vector2f velocity;
    float max_force;
    float max_speed;

    Vehicle(float m, Vector2f pos, Vector2f vel, float maxF, float maxS) 
        : mass(m), position(pos), velocity(vel), max_force(maxF), max_speed(maxS) {}
};

float length (Vector2f v) {
    return sqrt(v.x * v.x + v.y * v.y);
}

Vector2f normalize(Vector2f v) {
    float l = length(v);
    return (l > 0) ? Vector2f(v.x / l, v.y / l) : Vector2f(0, 0);
}

Vector2f limit(Vector2f v, float max_length) {
    float l = length(v);
    if (l > max_length) {
        return normalize(v) * max_length;
    } else {
        return v;
    }
}

void update(Vector2f steering, Vehicle& vehicle) {
    steering = limit(steering, vehicle.max_force);
    steering /= vehicle.mass;

    vehicle.velocity += steering;
    vehicle.velocity = limit(vehicle.velocity, vehicle.max_speed);
    vehicle.position += vehicle.velocity;
}

void seek(bool flee, Vehicle& vehicle, Vector2f target_pos) {
    Vector2f desired_velocity = normalize(target_pos - vehicle.position) * vehicle.max_speed;
    Vector2f steering = desired_velocity - vehicle.velocity;
    update(flee ? -steering : steering, vehicle);
}

void pursuit(bool evasion, Vehicle& vehicle, Vector2f target_pos) {
    Vector2f target_direction = target_pos - vehicle.position;
    
    Vector2f vehicle_velocity_normalized = normalize(vehicle.velocity);
    Vector2f target_direction_normalized = normalize(target_direction);

    float forward_alignment = (vehicle_velocity_normalized.x * target_direction_normalized.x) +
                              (vehicle_velocity_normalized.y * target_direction_normalized.y);

    float position_alignment = (target_direction_normalized.x * vehicle_velocity_normalized.x) +
                               (target_direction_normalized.y * vehicle_velocity_normalized.y);

    float distance = length(target_direction);
    float T = (distance * std::abs(forward_alignment * position_alignment)) / vehicle.max_speed;

    Vector2f target_velocity(Mouse::getPosition().x - target_pos.x, Mouse::getPosition().y - target_pos.y);
    target_velocity = normalize(target_velocity) * vehicle.max_speed;
    
    Vector2f future_position = target_pos + target_velocity * T;

    Vector2f direction = future_position - vehicle.position;
    Vector2f desired_velocity = normalize(direction) * vehicle.max_speed;

    Vector2f steering = desired_velocity - vehicle.velocity;

    update(evasion ? -steering : steering, vehicle);
}


void arrival(Vehicle& vehicle, Vector2f target_pos) {
    Vector2f target_offset = target_pos - vehicle.position;
    float distance = length(target_offset);

    float clipped_speed;
    if (distance < slowing_distance) {
        float ramped_speed = vehicle.max_speed * (distance / slowing_distance);
        clipped_speed = std::min(ramped_speed, vehicle.max_speed);
    } else {
        clipped_speed = vehicle.max_speed;
    }

    Vector2f desired_velocity = normalize(target_offset) * clipped_speed;
    Vector2f steering = desired_velocity - vehicle.velocity;
    update(steering, vehicle);
}

void circuit(Vehicle& vehicle, int& current_target_index) {
    Vector2f target_offset = path_points[current_target_index] - vehicle.position;
    float distance = length(target_offset);

    Vector2f target_pos = path_points[current_target_index];
    seek(false, vehicle, path_points[current_target_index]);

    if (distance < 10) {
        current_target_index = (current_target_index + 1) % path_points.size();
    }
}

void one_way(Vehicle& vehicle, int& current_target_index) {
    Vector2f target_offset = path_points[current_target_index] - vehicle.position;
    float distance = length(target_offset);

    Vector2f target_pos = path_points[current_target_index];

    if (current_target_index < path_points.size() - 1) {
        seek(false, vehicle, path_points[current_target_index]);
        if (distance < 10) {
            current_target_index = (current_target_index + 1) % path_points.size();
        }
    } else {
        arrival(vehicle, path_points[current_target_index]);
    }
}

void two_way(Vehicle& vehicle, int& current_target_index, bool& inverse, float& arrival_timer) {
    Vector2f target_offset = path_points[current_target_index] - vehicle.position;
    float distance = length(target_offset);

    Vector2f target_pos = path_points[current_target_index];

    bool at_end = (current_target_index == 0 && inverse) || (current_target_index == path_points.size() - 1 && !inverse);

    if (at_end) {
        arrival(vehicle, target_pos);

        if (length(vehicle.velocity) < 0.05f) { 
            arrival_timer -= 0.1f;
        }

        if (arrival_timer <= 0) {
            inverse = !inverse;
            arrival_timer = 2.0f;
        }
    } else {
        seek(false, vehicle, target_pos);
        if (distance < 10) {
            current_target_index += (inverse ? -1 : 1);
        }
    }
}

int main() {
    Vehicle vehicle(10, {100, 100}, {1.0f, 1.0f}, 1.0f, 5.0f);
    string mode = "Seek";
    int current_target_index = 0;
    bool inverse = false;
    float arrival_timer = 2.0f;
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Steering Simulation");
    window.setFramerateLimit(60);

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

    sf::Texture bee_texture;
    if (!bee_texture.loadFromFile("Bee.png")) {
        cerr << "Erreur : Impossible de charger l'image Bee.jpeg" << endl;
        return -1;
    }

    sf::Sprite bee_sprite(bee_texture);
    bee_sprite.setOrigin(bee_texture.getSize().x / 2, bee_texture.getSize().y / 2);
    bee_sprite.setScale(0.1f, 0.1f);

    sf::Texture flower_texture;
    if (!flower_texture.loadFromFile("Flower.png")) {
        cerr << "Erreur : Impossible de charger l'image Flower.jpg" << endl;
        return -1;
    }

    sf::Sprite flower_sprite(flower_texture);
    flower_sprite.setOrigin(flower_texture.getSize().x / 2, flower_texture.getSize().y / 2);
    flower_sprite.setScale(0.1f, 0.1f);

    flower_sprite.setScale(0.02f, 0.02f);

    
    sf::Texture bird_texture;
    if (!bird_texture.loadFromFile("Bird.png")) {
        cerr << "Erreur : Impossible de charger l'image Flower.jpg" << endl;
        return -1;
    }

    sf::Sprite bird_sprite(bird_texture);
    bird_sprite.setOrigin(bird_texture.getSize().x / 2, bird_texture.getSize().y / 2);
    bird_sprite.setScale(0.1f, 0.1f);



    sf::Texture beehive_texture;
    if (!beehive_texture.loadFromFile("Beehive.png")) {
        cerr << "Erreur : Impossible de charger l'image Flower.jpg" << endl;
        return -1;
    }

    sf::Sprite beehive_sprite(beehive_texture);
    beehive_sprite.setOrigin(beehive_texture.getSize().x / 2, beehive_texture.getSize().y / 2);
    beehive_sprite.setScale(0.1f, 0.1f);

    sf::Texture lake_texture;
    if (!lake_texture.loadFromFile("Lake.png")) {
        cerr << "Erreur : Impossible de charger l'image Flower.jpg" << endl;
        return -1;
    }

    sf::Sprite lake_sprite(lake_texture);
    lake_sprite.setOrigin(lake_texture.getSize().x / 2, lake_texture.getSize().y / 2);
    lake_sprite.setScale(0.4f, 0.4f);



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

        if (mode == "Seek") seek(false, vehicle, target_pos);
        else if (mode == "Flee") seek(true, vehicle, target_pos);
        else if (mode == "Pursuit") pursuit(false, vehicle, target_pos);
        else if (mode == "Evasion") pursuit(true, vehicle, target_pos);
        else if (mode == "Arrival") arrival(vehicle, target_pos);
        else if (mode == "Circuit") circuit(vehicle, current_target_index);
        else if (mode == "One Way") one_way(vehicle, current_target_index);
        else if (mode == "Two Way") two_way(vehicle, current_target_index, inverse, arrival_timer);

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
                    Sprite flowerOnPath(flower_texture);
                    flowerOnPath.setOrigin(flower_texture.getSize().x / 2, flower_texture.getSize().y / 2);
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

        if (length(vehicle.velocity) > 0.1f) {
            float angle = atan2(vehicle.velocity.y, vehicle.velocity.x) * 180 / M_PI;
            bee_sprite.setRotation(angle);
        }
        
        window.draw(bee_sprite);

        window.draw(modeText);

        window.display();
    }

    return 0;
}
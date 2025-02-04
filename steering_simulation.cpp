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

Vector2f normalize(Vector2f v) {
    float length = sqrt(v.x * v.x + v.y * v.y);
    return (length > 0) ? Vector2f(v.x / length, v.y / length) : Vector2f(0, 0);
}

void update(Vector2f steering, Vehicle& vehicle) {
    steering = normalize(steering) * vehicle.max_force;
    steering /= vehicle.mass;

    vehicle.velocity += steering;
    vehicle.velocity = normalize(vehicle.velocity) * vehicle.max_speed;
    vehicle.position += vehicle.velocity;
}

void seek(bool flee, Vehicle& vehicle, Vector2f target_pos) {
    Vector2f desired_velocity = normalize(target_pos - vehicle.position) * vehicle.max_speed;
    Vector2f steering = desired_velocity - vehicle.velocity;
    update(flee ? -steering : steering, vehicle);
}

int main() {
    Vehicle vehicle(10, {100, 100}, {0.2f, 0.2f}, 1.0f, 0.1f);
    String mode = "seek";
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Steering Simulation");

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed)
                if (event.key.code == sf::Keyboard::S) mode = "seek";
                if (event.key.code == sf::Keyboard::F) mode = "flee";
            }

        Vector2f target_pos = Vector2f(Mouse::getPosition(window));

        if (mode == "seek") seek(false, vehicle, target_pos);
        else if (mode == "flee") seek(true, vehicle, target_pos);

        window.clear(Color::Green);
        CircleShape vehicleShape(50);
        vehicleShape.setFillColor(Color::Yellow);
        vehicleShape.setPosition(vehicle.position);
        window.draw(vehicleShape);

        window.display();
    }

    return 0;
}
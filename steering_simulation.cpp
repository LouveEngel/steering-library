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
    {100, 100}, {350, 100}, {600, 100}, {600, 600}, {350, 600}, {100, 600}
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

Vector2f normalize(Vector2f v) {
    float length = sqrt(v.x * v.x + v.y * v.y);
    return (length > 0) ? Vector2f(v.x / length, v.y / length) : Vector2f(0, 0);
}

float length (Vector2f v) {
    return sqrt(v.x * v.x + v.y * v.y);
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

void arrival(Vehicle& vehicle, Vector2f target_pos) {
    Vector2f target_offset = target_pos - vehicle.position;
    float distance = sqrt(target_offset.x * target_offset.x + target_offset.y * target_offset.y);

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
    float distance = sqrt(target_offset.x * target_offset.x + target_offset.y * target_offset.y);

    Vector2f target_pos = path_points[current_target_index];
    seek(false, vehicle, path_points[current_target_index]);

    if (distance < 10) {
        current_target_index = (current_target_index + 1) % path_points.size();
    }
}

void one_way(Vehicle& vehicle, int& current_target_index) {
    Vector2f target_offset = path_points[current_target_index] - vehicle.position;
    float distance = sqrt(target_offset.x * target_offset.x + target_offset.y * target_offset.y);

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

void two_way(Vehicle& vehicle, int& current_target_index, bool& inverse) {
    Vector2f target_offset = path_points[current_target_index] - vehicle.position;
    float distance = sqrt(target_offset.x * target_offset.x + target_offset.y * target_offset.y);

    Vector2f target_pos = path_points[current_target_index];
    seek(false, vehicle, path_points[current_target_index]);

    if (distance < 10) {
        if (inverse) {
            if (current_target_index > 0) {
                current_target_index -= 1;
            } else {
                    inverse = false;
            }
        } else {
            if (current_target_index < path_points.size() - 1) {
                current_target_index += 1;
            } else {
                inverse = true;
            }
        }
    }
}

int main() {
    Vehicle vehicle(10, {100, 100}, {1.0f, 1.0f}, 1.0f, 5.0f);
    string mode = "seek";
    int current_target_index = 0;
    bool inverse = false;
    RenderWindow window(VideoMode(WIDTH, HEIGHT), "Steering Simulation");
    window.setFramerateLimit(60);

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::KeyPressed) {
                if (event.key.code == sf::Keyboard::S) mode = "seek";
                if (event.key.code == sf::Keyboard::F) mode = "flee";
                if (event.key.code == sf::Keyboard::A) mode = "arrival";
                if (event.key.code == sf::Keyboard::C) mode = "circuit";
                if (event.key.code == sf::Keyboard::O) mode = "one_way";
                if (event.key.code == sf::Keyboard::T) mode = "two_way";
            }
        }

        Vector2f target_pos = Vector2f(Mouse::getPosition(window));

        if (mode == "seek") seek(false, vehicle, target_pos);
        else if (mode == "flee") seek(true, vehicle, target_pos);
        else if (mode == "arrival") arrival(vehicle, target_pos);
        else if (mode == "circuit") circuit(vehicle, current_target_index);
        else if (mode == "one_way") one_way(vehicle, current_target_index);
        else if (mode == "two_way") two_way(vehicle, current_target_index, inverse);

        window.clear(Color::Green);
        CircleShape vehicleShape(25);
        vehicleShape.setFillColor(Color::Yellow);
        vehicleShape.setOrigin(vehicleShape.getRadius(), vehicleShape.getRadius());
        vehicleShape.setPosition(vehicle.position);
        window.draw(vehicleShape);

        for (const auto& point : path_points) {
            CircleShape pointShape(5);
            pointShape.setFillColor(Color::Blue);
            pointShape.setOrigin(5, 5);
            pointShape.setPosition(point);
            window.draw(pointShape);
        }


        window.display();
    }

    return 0;
}
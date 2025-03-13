#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <set>
#include <queue>
#include <vector>
#include <functional>

using namespace std;
using namespace sf;

// Classe representant un vehicule avec des caracteristiques physiques et des comportements de mouvement
class Vehicle {

public:
    float mass; // Masse du vehicule
    Vector2f position; // Position actuelle du vehicule
    Vector2f velocity; // Vitesse actuelle du vehicule
    float max_force; // Force maximale pouvant etre appliquee
    float max_speed;


    // Constructeur initialisant les proprietes du vehicule
    Vehicle(float m, Vector2f pos, Vector2f vel, float maxF, float maxS) 
        : mass(m), position(pos), velocity(vel), max_force(maxF), max_speed(maxS) {}


    // Cette fonction permet de connaitre la longueur d'un vecteur
    float length (Vector2f v) {
        return sqrt(v.x * v.x + v.y * v.y);
    }


    // Cette fonction permet de normaliser un vecteur (le ramene a une longueur de 1 tout en conservant sa direction)
    Vector2f normalize(Vector2f v) {
        float l = length(v);
        return (l > 0) ? Vector2f(v.x / l, v.y / l) : Vector2f(0, 0);
    }


    // Cette fonction permet de limiter la longueur d'un vecteur a une valeur maximale
    Vector2f limit(Vector2f v, float max_length) {
        float l = length(v);
        return (l > max_length) ? normalize(v) * max_length : v;
    }


    // Cette fonction permet de mettre a jour la position et la vitesse du vehicule en fonction de la force de direction appliquee
    void update(Vector2f steering) {
        steering = limit(steering, max_force);
        steering /= mass;

        velocity += steering;
        velocity = limit(velocity, max_speed);
        position += velocity;
    }


    // Cette fonction permet de simuler un comportement de recherche de cible (ou fuite si flee est a true)
    void seek(bool flee, Vector2f target_pos) {
        Vector2f desired_velocity = normalize(target_pos - position) * max_speed;
        Vector2f steering = desired_velocity - velocity;
        update(flee ? -steering : steering);
    }


    // Cette fonction permet de simuler un comportement de poursuite (ou d'evitement si evasion est a true)
    void pursuit(bool evasion, Vector2f target_pos) {
        Vector2f target_direction = target_pos - position;
        
        Vector2f vehicle_velocity_normalized = normalize(velocity);
        Vector2f target_direction_normalized = normalize(target_direction);

        float forward_alignment = (vehicle_velocity_normalized.x * target_direction_normalized.x) +
                                (vehicle_velocity_normalized.y * target_direction_normalized.y);

        float position_alignment = (target_direction_normalized.x * vehicle_velocity_normalized.x) +
                                (target_direction_normalized.y * vehicle_velocity_normalized.y);

        float distance = length(target_direction);
        float T = (distance * std::abs(forward_alignment * position_alignment)) / max_speed;

        Vector2f target_velocity(Mouse::getPosition().x - target_pos.x, Mouse::getPosition().y - target_pos.y);
        target_velocity = normalize(target_velocity) * max_speed;
        
        Vector2f future_position = target_pos + target_velocity * T;

        Vector2f direction = future_position - position;
        Vector2f desired_velocity = normalize(direction) * max_speed;

        Vector2f steering = desired_velocity - velocity;

        update(evasion ? -steering : steering);
    }


    // Cette fonction permet de simuler un comportement d'approche d'une cible avec ralentissement
    void arrival(Vector2f target_pos, float slowing_distance) {
        Vector2f target_offset = target_pos - position;
        float distance = length(target_offset);

        float clipped_speed;
        if (distance < slowing_distance) {
            float ramped_speed = max_speed * (distance / slowing_distance);
            clipped_speed = std::min(ramped_speed, max_speed);
        } else {
            clipped_speed = max_speed;
        }

        Vector2f desired_velocity = normalize(target_offset) * clipped_speed;
        Vector2f steering = desired_velocity - velocity;
        update(steering);
    }


    // Cette fonction permet de simuler le suivi d'un chemin circulaire
    void circuit(int& current_target_index, vector<Vector2f>& path_points) {
        Vector2f target_offset = path_points[current_target_index] - position;
        float distance = length(target_offset);

        Vector2f target_pos = path_points[current_target_index];
        seek(false, path_points[current_target_index]);

        if (distance < 10) {
            current_target_index = (current_target_index + 1) % path_points.size();
        }
    }


    // Cette fonction permet de simuler le suivi d'un chemin en sens unique avec ralentissement a la fin
    void one_way(int& current_target_index, vector<Vector2f>& path_points, float slowing_distance) {
        Vector2f target_offset = path_points[current_target_index] - position;
        float distance = length(target_offset);

        Vector2f target_pos = path_points[current_target_index];

        if (current_target_index < path_points.size() - 1) {
            seek(false, path_points[current_target_index]);
            if (distance < 10) {
                current_target_index = (current_target_index + 1) % path_points.size();
            }
        } else {
            arrival(path_points[current_target_index], slowing_distance);
        }
    }


    // Cette fonction permet de simuler un aller-retour sur un chemin avec un temps d'arret aux extremites
    void two_way(int& current_target_index, bool& inverse, float& arrival_timer, vector<Vector2f>& path_points, float slowing_distance) {
        Vector2f target_offset = path_points[current_target_index] - position;
        float distance = length(target_offset);

        Vector2f target_pos = path_points[current_target_index];

        bool at_end = (current_target_index == 0 && inverse) || (current_target_index == path_points.size() - 1 && !inverse);

        if (at_end) {
            arrival(target_pos, slowing_distance);

            if (length(velocity) < 0.05f) { 
                arrival_timer -= 0.1f;
            }

            if (arrival_timer <= 0) {
                inverse = !inverse;
                arrival_timer = 2.0f;
            }
        } else {
            seek(false, target_pos);
            if (distance < 10) {
                current_target_index += (inverse ? -1 : 1);
            }
        }
    }
};
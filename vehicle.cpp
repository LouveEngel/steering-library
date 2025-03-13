#include <SFML/Graphics.hpp>
#include <SFML/System.hpp>
#include <SFML/Window.hpp>
#include <cmath>
#include <set>
#include <queue>
#include <vector>
#include <functional>

#include "common_functions.cpp" // Inclusion des fonctions communes de l'application

using namespace std;
using namespace sf;

// Classe representant un vehicule avec des caracteristiques physiques et des comportements de mouvement
class Vehicle {

public:
    float mass; // Masse du vehicule
    Vector2f position; // Position actuelle du vehicule
    Vector2f velocity; // Vitesse actuelle du vehicule
    float max_force; // Force maximale pouvant etre appliquee
    float max_speed; // Vitesse maximale autorisee pour le vehicule
    int bestIndex = -1; // Index du meilleur point cible (initialise a -1 : aucun)
    bool carryingFlower = false; // Indique si le vehicule transporte une fleur
    vector<int> optimalPath; // Chemin optimal a suivre (liste d'indices de points)
    int pathStep = 0; // Etape actuelle dans le suivi du chemin optimal


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


    // Fonction template generique pour trouver l'indice du point le plus proche d'un point de reference
    template <typename T>
    int findClosestIndex(const std::vector<T>& points, const Vector2f& ref, std::function<bool(const T&)> isValid, std::function<Vector2f(const T&)> getPosition) {
        int bestIndex = -1; // Index du point le plus proche (initialise a -1 : non trouve)
        float bestDist = std::numeric_limits<float>::max(); // Distance minimale initialisee a l'infini

        for (size_t i = 0; i < points.size(); i++) {
            // Verifie si le point est valide selon le predicat
            if (isValid(points[i])) {
                // Recupere la position du point
                Vector2f p = getPosition(points[i]);

                // Calcule la distance au carre entre la reference et le point
                float dx = ref.x - p.x;
                float dy = ref.y - p.y;
                float d = dx * dx + dy * dy;

                // Si ce point est plus proche, met a jour les valeurs
                if (d < bestDist) {
                    bestDist = d;
                    bestIndex = i;
                }
            }
        }
        return bestIndex;
    }


    // Recherche de la fleur la plus proche qui n'a pas encore ete ramassee ni livree
    int findClosestPointIndex(const std::vector<Flower>& points) {
        return findClosestIndex<Flower>(
            points,
            position,
            [](const Flower& f) { return !f.picked && !f.delivered; },
            [](const Flower& f) { return f.sprite.getPosition(); }
        );
    }


    // Recherche du point de chemin le plus proche d'une reference parmi les points du chemin
    int findClosestPathPointIndex(const Vector2f& ref, const std::vector<Vector2f>& path_points) {
        return findClosestIndex<Vector2f>(
            path_points,
            ref,
            [](const Vector2f&) { return true; }, // Tous les points sont valides
            [](const Vector2f& pos) { return pos; }
        );
    }


    // Recherche du chemin le plus court dans un graphe en utilisant le parcours en largeur (BFS)
    std::vector<int> findShortestPath(int start, int goal, std::map<int, std::vector<int>>& graph) {
        std::queue<std::vector<int>> paths; // File pour stocker les chemins partiels
        std::set<int> visited; // Ensemble pour suivre les points deja visites

        // Demarrage du parcours avec le point de depart
        paths.push({start});
        visited.insert(start);

        // Parcours en largeur
        while (!paths.empty()) {
            // Recupere le chemin en tete de file
            std::vector<int> path = paths.front();
            paths.pop();

            // Recupere le dernier point du chemin actuel
            int current = path.back();
            // Si la cible est atteinte, retourne le chemin complet
            if (current == goal) {
                return path;
            }

            // Pour chaque voisin du point courant
            for (int neighbor : graph[current]) {
                // S'il n'a pas encore ete visite
                if (visited.find(neighbor) == visited.end()) {
                    visited.insert(neighbor);
                    // Cree un nouveau chemin en ajoutant le voisin
                    std::vector<int> newPath = path;
                    newPath.push_back(neighbor);
                    paths.push(newPath);
                }
            }
        }
        return {}; // Aucun chemin trouve, retourne un vecteur vide
    }


    // Methode de mise a jour de la navigation pour l'abeille controlee par l'IA
    void updateAI(const std::vector<Vector2f>& path_points, const std::map<int, std::vector<int>>& graph, std::vector<Flower>& flowers, const Vector2f& hivePos, float slowing_distance) {

        // Lambda pour recalculer le chemin optimal si la cible change
        auto recalcPathIfNeeded = [&](int goalIndex) {
            // Determine le point de chemin le plus proche de la position actuelle
            int startIndex = findClosestPathPointIndex(position, path_points);

            // Si aucun chemin optimal n'a ete defini ou si le dernier point du chemin ne correspond pas a la nouvelle cible
            if (optimalPath.empty() || optimalPath.back() != goalIndex) {

                // Necessite un const_cast car le graphe est passe en parametre constant
                optimalPath = findShortestPath(startIndex, goalIndex, const_cast<std::map<int, std::vector<int>>&>(graph));
                pathStep = 0;
            }
        };

        // Lambda pour suivre le chemin optimal point par point
        auto followPath = [&]() {
            if (!optimalPath.empty() && pathStep < optimalPath.size()) {

                // Recupere la position du prochain point du chemin
                Vector2f nextPos = path_points[optimalPath[pathStep]];

                // Se dirige vers ce point en appliquant le comportement d'arrivee (avec ralentissement)
                arrival(nextPos, slowing_distance);

                // Si le vehicule est suffisamment proche (distance au carre < 100), passe a l'etape suivante
                float dx = position.x - nextPos.x;
                float dy = position.y - nextPos.y;
                if ((dx * dx + dy * dy) < 100.0f) {
                    pathStep++;
                }
            }
        };

        if (!carryingFlower) {

            // Si l'IA ne transporte pas de fleur, elle doit en chercher une
            bool flowerAvailable = false;
            // Verifie s'il existe au moins une fleur non ramassee et non livree
            for (const auto &f : flowers) {
                if (!f.picked && !f.delivered) {
                    flowerAvailable = true;
                    break;
                }
            }

            if (!flowerAvailable) {
                // Si aucune fleur n'est disponible, se diriger vers la ruche
                int goalIndex = findClosestPathPointIndex(hivePos, path_points);
                recalcPathIfNeeded(goalIndex);
                followPath();
                // Une fois le chemin termine, utilise seek pour aller directement vers la ruche
                if (pathStep >= optimalPath.size()) {
                    seek(false, hivePos);
                }
                return; // Fin de la mise a jour
            }

            // Si aucune fleur n'est ciblee ou si la fleur ciblee a ete prise par le joueur, recalcul de la cible
            if (bestIndex == -1 || (flowers[bestIndex].picked && flowers[bestIndex].carrier == Carrier::Player)) {
                bestIndex = findClosestPointIndex(flowers);
                if (bestIndex == -1)
                    return; // Aucune fleur disponible
                
                // Determine la position de la fleur cible
                Vector2f targetFlowerPos = flowers[bestIndex].sprite.getPosition();
                // Trouve le point de chemin le plus proche de la fleur cible
                int goalIndex = findClosestPathPointIndex(targetFlowerPos, path_points);
                recalcPathIfNeeded(goalIndex);
            }
            // Suivi du chemin optimal vers la fleur cible
            followPath();

        } else {
            // Si l'IA transporte une fleur, elle doit retourner a la ruche
            int goalIndex = findClosestPathPointIndex(hivePos, path_points);
            recalcPathIfNeeded(goalIndex);
            followPath();

            // Une fois le chemin termine, se dirige directement vers la ruche
            if (pathStep >= optimalPath.size()) {
                seek(false, hivePos);
            }
        }
    }
};
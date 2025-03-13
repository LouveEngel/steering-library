#include <SFML/Graphics.hpp> // Bibliotheque pour l'affichage graphique
#include <SFML/System.hpp>   // Bibliotheque pour les fonctionnalites systeme
#include <SFML/Window.hpp>   // Bibliotheque pour la gestion des evenements
#include <vector>
#include <map>

using namespace sf;
using namespace std;

// Enumeration qui definit qui peut porter une fleur
enum class Carrier { None, Player, AI };

// Structure representant une fleur dans le jeu
struct Flower {
    Sprite sprite; // Represente l'image graphique de la fleur
    bool picked = false; // Indique si la fleur a ete cueillie (true) ou non (false)
    bool delivered = false; // Indique si la fleur a ete livree a la ruche (true) ou non (false)
    Carrier carrier = Carrier::None; // Identifie le porteur de la fleur : aucun, le joueur ou l'IA
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

// Fonction qui cree un rectangle et l'ajoute a un vecteur de RectangleShape
void createRect(std::vector<RectangleShape>& path, sf::Color path_color, int sizeX, int sizeY, int positionX, int positionY) {
    RectangleShape rect;
    rect.setSize(Vector2f(sizeX, sizeY));
    rect.setFillColor(path_color);
    rect.setPosition(positionX, positionY);
    path.push_back(rect);
}
enum class Carrier { None, Player, AI };

struct Flower {
    Sprite sprite;
    bool picked = false;
    bool delivered = false;
    Carrier carrier = Carrier::None;
};

// Cette fonction permet de charger une texture a partir d'un fichier
void loadTexture(map<string, Texture>& textures, const string& name, const string& path) {
    if (!textures[name].loadFromFile(path)) {
        cerr << "Erreur : Impossible de charger " << path << endl;
        exit(-1);
    }
}

void createRect(std::vector<RectangleShape>& path, sf::Color path_color, int sizeX, int sizeY, int positionX, int positionY) {
    RectangleShape rect;
    rect.setSize(Vector2f(sizeX, sizeY));
    rect.setFillColor(path_color);
    rect.setPosition(positionX, positionY);
    path.push_back(rect);
}
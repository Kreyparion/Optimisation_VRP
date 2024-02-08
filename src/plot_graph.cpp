#include "matplotlibcpp.h"
namespace plt = matplotlibcpp;

int main() {
    // Exemple de coordonnées des nœuds (x, y)
    std::vector<double> x_coords = {1, 2, 3, 4}; // Coordonnées x des nœuds
    std::vector<double> y_coords = {1, 3, 2, 4}; // Coordonnées y des nœuds

    // Exemple de trajets : de 0 à 1, de 1 à 2, etc.
    std::vector<std::pair<int, int>> trajets = {{0, 1}, {1, 2}, {2, 3}};

    // Tracer les nœuds
    plt::scatter(x_coords, y_coords);

    // Tracer les trajets
    for (const auto& trajet : trajets) {
        std::vector<double> x_line = {x_coords[trajet.first], x_coords[trajet.second]};
        std::vector<double> y_line = {y_coords[trajet.first], y_coords[trajet.second]};
        plt::plot(x_line, y_line);
    }

    // Afficher le graphe
    plt::show();
}

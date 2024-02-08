#include <iostream>
#include <vector>

int main_() {
    const int numNodes = 5; // Nombre total de nœuds
    std::vector<bool> activeEdges(numNodes - 1, false); // État des arêtes: active (true) ou non (false)

    // Exemple de configuration des trajets (arêtes actives)
    // Pour simplifier, on utilise l'index de l'arête: 0 pour l'arête entre le nœud 0 et 1, etc.
    activeEdges[1] = true; // Arête entre le nœud 1 et 2 est empruntée
    activeEdges[3] = true; // Arête entre le nœud 3 et 4 est empruntée

    // Afficher le graphe
    for (int i = 0; i < numNodes; ++i) {
        std::cout << "*"; // Nœud
        if (i < numNodes - 1) { // Si ce n'est pas le dernier nœud, afficher l'arête
            if (activeEdges[i]) {
                std::cout << " -> "; // Arête empruntée
            } else {
                std::cout << " -- "; // Arête non empruntée
            }
        }
    }

    std::cout << std::endl;

    return 0;
}

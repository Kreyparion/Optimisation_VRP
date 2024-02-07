#include <ilcplex/ilocplex.h>


//On teste un modèle simple avce deux instances dont une instance a une contrainte de définition, deux contraintes et une fonction objectif.

int main() {
    IloEnv env;
    try {

        //Etape1: Créer modèle (IloModel)
        // Créer un modèle
        IloModel model(env);

        // Définir les variables
        IloNumVar x(env, 0, 40); // x avec une limite inférieure de 0 et une limite supérieure de 40
        IloNumVar y(env); // y sans limites spécifiques

        // Ajouter les variables au modèle
        model.add(x);
        model.add(y);

        // Créer une expression objective et l'ajouter au modèle
        model.add(IloMaximize(env, 1*x + 2*y)); // Maximiser x + 2y

        // Ajouter des contraintes
        model.add( -1*x + 1*y <= 20);
        model.add( 1*x + 1*y <= 40);

        //Etape 2: créer solveur sur le modèle(IloCplex)
        // Créer et utiliser un solveur pour résoudre le modèle
        IloCplex cplex(model);
        if (cplex.solve()) {
            // Afficher la solution
            std::cout << "Solution status = " << cplex.getStatus() << std::endl;
            std::cout << "Solution value  = " << cplex.getObjValue() << std::endl;
            std::cout << "x = " << cplex.getValue(x) << std::endl;
            std::cout << "y = " << cplex.getValue(y) << std::endl;
        }
    }
    catch (IloException& e) {
        std::cerr << "Exception caught: " << e << std::endl;
    }
    env.end();
    return 0;
}

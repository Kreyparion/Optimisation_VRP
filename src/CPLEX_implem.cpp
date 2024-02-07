#pragma once
#include <ilcplex/ilocplex.h>
#include "config.h"

ILOSTLBEGIN
// Decision variables
IloIntVarArray x; // x[i][j][k] = 1 if vehicle k travels from i to j
IloIntVarArray y; // y[i][k] = 1 if Short-term vehicle k visits customer i
IloIntVarArray d; // d[k] = distance penalty for vehicle k
IloIntVarArray t; // t[k] = time penalty for vehicle k
IloIntVarArray u; // u[i][k] = variable for subtour elimination for vehicle k


int opti(Config config){
    IloEnv env;
   try {
        IloInt nb_max_courgettes = 3;
        IloNumVar nbNavets(env, 0, IloInfinity, ILOINT);
        IloNumVar nbCourgettes(env, 0, nb_max_courgettes, ILOINT);
        
        IloModel model(env);

        model.add(IloMaximize(env, 4*nbNavets + 5*nbCourgettes));
        model.add(2*nbNavets + nbCourgettes <= 8);
        model.add(nbNavets + 2*nbCourgettes <= 7);

        IloCplex cplex(model);
        cplex.solve();

        IloNumArray vals(env);
        env.out() << "Cost:" << cplex.getObjValue() << std::endl;
        env.out() << "nbNavets = " << cplex.getValue(nbNavets) << std::endl;
        env.out() << "nbCourgettes = " << cplex.getValue(nbCourgettes) << std::endl;
    }
    catch (IloException& ex) {
      std::cerr << "Error: " << ex << std::endl;
   }
   catch (...) {
      std::cerr << "Error" << std::endl;
   }
   env.end();
   return 0;

}
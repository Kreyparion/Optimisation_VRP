#include <ilcplex/ilocplex.h>


int main(){
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
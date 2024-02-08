#pragma once
#include <ilcplex/ilocplex.h>
#include "config.h"

ILOSTLBEGIN
// Decision variables
IloIntVarArray x; // x[i][j][k] = 1 if vehicle k travels from i to j
IloIntVarArray y; // y[i][k] = 1 if Short-term vehicle k visits customer i
IloNumVarArray d; // d[k] = distance penalty for vehicle k
IloNumVarArray t; // t[k] = time penalty for vehicle k
IloNumVarArray u; // u[i][k] = variable for subtour elimination for vehicle k


int opti(Config config){
    IloEnv env;
   try {
        x = IloIntVarArray(env, config.nbVertex*config.nbVertex*config.nbVehicle, 0, 1);
        y = IloIntVarArray(env, config.nbVertex*config.nbShortTermVehicle, 0, 1);
        d = IloNumVarArray(env, config.nbVehicle, 0, IloInfinity);
        t = IloNumVarArray(env, config.nbVehicle, 0, IloInfinity);
        u = IloNumVarArray(env, config.nbVertex*config.nbVehicle, 0, config.nbVertex-1);


        IloModel model(env);

        //objective function
        IloExpr cost(env);
        for(int k = 0; k < config.nbVehicle; k++){
            cost += config.distancePenalty[k]*d[k] + config.timePenalty[k]*t[k];
            for(int i=0; i<config.nbVertex; i++){
                cost += config.fixedCostShortTermVehicle[k]*y[i*config.nbVehicle + k];
            }
            for(int j=1; j<config.nbVertex; j++){
                cost += config.fixedCostVehicle[k]*x[j*config.nbVehicle + k];
            }
        }

        model.add(IloMinimize(env, cost));
        
        //constraints

        // Diagonal constraint
        for(int k = 0; k < config.nbVehicle; k++){
            for(int i=0; i<config.nbVertex; i++){
                for(int j=0; j<config.nbVertex; j++){
                    if(i == j){
                        model.add(x[i*config.nbVehicle*config.nbVertex + j*config.nbVehicle + k] == 0);
                    }
                }
            }
        }
        for(int k = 0; k < config.nbShortTermVehicle; k++){
            model.add(y[0*config.nbShortTermVehicle + k] == 0);
        }

        // Soft and Hard Time limit constraint
        for(int k = 0; k < config.nbVehicle; k++){
            IloExpr time(env);  
            for(int i=0; i<config.nbVertex; i++){
                for(int j=0; j<config.nbVertex; j++){
                    time += config.dist[i*config.nbVertex+j]/config.speed[k]*x[i*config.nbVehicle*config.nbVertex + j*config.nbVehicle + k];
                }
            }
            model.add(time <= config.HardTimeLimit[k]);
            model.add(time - config.SoftTimeLimit[k] <= t[k]);
        }

        // Soft Distance limit constraint
        for(int k = 0; k < config.nbVehicle; k++){
            IloExpr distance(env);
            for(int i=0; i<config.nbVertex; i++){
                for(int j=0; j<config.nbVertex; j++){
                    distance += config.dist[i*config.nbVertex+j]*x[i*config.nbVehicle*config.nbVertex + j*config.nbVehicle + k];
                }
            }
            model.add(distance - config.SoftDistanceLimit[k] <= d[k]);
        }
        
        // Hard Distance limit constraint for short-term vehicle
        for(int k = 0; k < config.nbShortTermVehicle; k++){
            IloExpr distance(env);
            for(int i=0; i<config.nbVertex; i++){
                distance += config.dist[0*config.nbVertex+i]*y[i*config.nbShortTermVehicle + k];
            }
            model.add(distance <= config.HardDistanceLimitShortTermVehicle[k]);
        }

        // Flow constraint
        for(int k = 0; k < config.nbVehicle; k++){
            for(int i=0; i<config.nbVertex; i++){
                IloExpr flow(env);
                for(int j=0; j<config.nbVertex; j++){
                    flow += x[i*config.nbVehicle*config.nbVertex + j*config.nbVehicle + k] - x[j*config.nbVehicle*config.nbVertex + i*config.nbVehicle + k];
                }
                model.add(flow == 0);
            }
        }

        // All customers must be visited by a short-term vehicle or a vehicle
        for(int j=1; j<config.nbVertex; j++){
            IloExpr visit(env);
            for(int k=0; k<config.nbVehicle; k++){
                for(int i=0; i<config.nbVertex; i++){
                    visit += x[i*config.nbVehicle*config.nbVertex + j*config.nbVehicle + k];
                }
            }
            for(int k=0; k<config.nbShortTermVehicle; k++){
                visit += y[j*config.nbShortTermVehicle + k];
            }
            model.add(visit == 1);
        }

        // Only use one Short-term vehicle
        for(int k=0; k<config.nbShortTermVehicle; k++){
            IloExpr use(env);
            for(int j=1; j<config.nbVertex; j++){
                use += y[j*config.nbShortTermVehicle + k];
            }
            model.add(use <= 1);
        }

        // Only use one vehicle
        for(int k=0; k<config.nbVehicle; k++){
            IloExpr use(env);
            for(int j=1; j<config.nbVertex; j++){
                use += x[j*config.nbVehicle + k];
            }
            model.add(use <= 1);
        }

        // Subtour elimination
        for(int k=0; k<config.nbVehicle; k++){
            for(int i=1; i<config.nbVertex; i++){
                for(int j=1; j<config.nbVertex; j++){
                    if(i != j){
                        model.add(u[i*config.nbVehicle + k] - u[j*config.nbVehicle + k] + x[i*config.nbVehicle*config.nbVertex + j*config.nbVehicle + k]*config.Capacity[k] <= config.Capacity[k] - config.Demand[j]);
                    }
                }
            }
        }

        // Subtour elimination variable bounds
        for(int k=0; k<config.nbVehicle; k++){
            for(int i=1; i<config.nbVertex; i++){
                model.add(config.Demand[i] <= u[i*config.nbVehicle + k]);
                model.add(u[i*config.nbVehicle + k] <= config.Capacity[k]);
            }
        }

        // Subtour elimination variable for base
        for(int k=0; k<config.nbVehicle; k++){
            model.add(u[0*config.nbVehicle + k] == 0);
        }
        

        IloCplex cplex(model);
        cplex.solve();

        IloNumArray vals(env);
        env.out() << "Cost:" << cplex.getObjValue() << std::endl;
        //env.out() << "Result = " << cplex.getValue(x) << std::endl;
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
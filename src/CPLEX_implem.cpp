#pragma once
#include <ilcplex/ilocplex.h>
#include "config.h"
#include <chrono>


ILOSTLBEGIN

// Decision variables
IloIntVarArray x; // x[i][j][k] = 1 if vehicle k travels from i to j
IloIntVarArray y; // y[i][k] = 1 if Short-term vehicle k visits customer i
IloNumVarArray d; // d[k] = distance penalty for vehicle k
IloNumVarArray t; // t[k] = time penalty for vehicle k
IloNumVarArray u; // u[i][k] = variable for subtour elimination for vehicle k

/**
 * @brief Optimize the problem using CPLEX
 * 
 * @param config The configuration of the problem
 * @param verbose The verbosity level between 0 and 2
 * @return float The best score found
 */
Solution opti(Config& config, int verbose = 0){
    IloEnv env;
    float best_score = 100000000.0;
    Solution solution = init_solution(config.nbVehicle + config.nbShortTermVehicle);
   try {
        // Bounds for the decision variables
        x = IloIntVarArray(env, config.nbVertex*config.nbVertex*config.nbVehicle, 0, 1);
        y = IloIntVarArray(env, config.nbVertex*config.nbShortTermVehicle, 0, 1);
        d = IloNumVarArray(env, config.nbVehicle, 0, IloInfinity);
        t = IloNumVarArray(env, config.nbVehicle, 0, IloInfinity);
        u = IloNumVarArray(env, config.nbVertex*config.nbVehicle, 0, IloInfinity);


        IloModel model(env);
        
        // Add decision variables to the model
        model.add(x);
        model.add(y);
        model.add(d);
        model.add(t);
        model.add(u);

        //------------------------------------- Objective function -------------------------------------
        IloExpr cost(env);
        for(int k = 0; k < config.nbShortTermVehicle; k++){
            for(int i=1; i<config.nbVertex; i++){
                cost += config.fixedCostShortTermVehicle[k]*y[i*config.nbShortTermVehicle + k];
            }
        }
        for(int k = 0; k < config.nbVehicle; k++){
            cost += config.distancePenalty[k]*d[k] + config.timePenalty[k]*t[k];
            for(int j=1; j<config.nbVertex; j++){
                cost += config.fixedCostVehicle[k]*x[0 +j*config.nbVehicle + k];
            }
        }

        model.add(IloMinimize(env, cost));

        //------------------------------------- Constraints -------------------------------------

        // Constraints for unused variables
        for(int k = 0; k < config.nbVehicle; k++){
            for(int i=0; i<config.nbVertex; i++){
                for(int j=0; j<config.nbVertex; j++){
                    if(i == j){
                        model.add(x[i*config.nbVehicle*config.nbVertex + j*config.nbVehicle + k] == 0.0);
                    }
                }
            }
        }
        for(int k = 0; k < config.nbShortTermVehicle; k++){
            model.add(y[0*config.nbShortTermVehicle + k] == 0.0);
        }

        // Soft and Hard Time limit constraints
        
        for(int k = 0; k < config.nbVehicle; k++){
            IloExpr time(env);
            for(int i=0; i<config.nbVertex; i++){
                for(int j=0; j<config.nbVertex; j++){
                    if(i != j){
                        time += (config.dist[i*config.nbVertex+j]/config.speed[k])*x[i*config.nbVehicle*config.nbVertex + j*config.nbVehicle + k];
                    }
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
                    if (i != j){
                        distance += config.dist[i*config.nbVertex+j]*x[i*config.nbVehicle*config.nbVertex + j*config.nbVehicle + k];
                    }
                }
            }
            model.add(distance - config.SoftDistanceLimit[k] <= d[k]);
        }
        
        // Hard Distance limit constraint for short-term vehicle
        for(int k = 0; k < config.nbShortTermVehicle; k++){
            IloExpr distance(env);
            for(int i=1; i<config.nbVertex; i++){
                distance += config.dist[0*config.nbVertex+i]*y[i*config.nbShortTermVehicle + k];
            }
            model.add(distance <= config.HardDistanceLimitShortTermVehicle[k]);
        }

        // Flow constraint
        for(int k = 0; k < config.nbVehicle; k++){
            for(int i=0; i<config.nbVertex; i++){
                IloExpr flow(env);
                for(int j=0; j<config.nbVertex; j++){
                    if (i != j){
                        flow += x[i*config.nbVehicle*config.nbVertex + j*config.nbVehicle + k] - x[j*config.nbVehicle*config.nbVertex + i*config.nbVehicle + k];
                    }
                }
                model.add(flow == 0.0);
            }
        }

        // All customers must be visited by a short-term vehicle or a vehicle
        for(int j=1; j<config.nbVertex; j++){
            IloExpr visit(env);
            for(int k=0; k<config.nbVehicle; k++){
                for(int i=0; i<config.nbVertex; i++){
                    if (i != j){
                        visit += x[i*config.nbVehicle*config.nbVertex + j*config.nbVehicle + k];
                    }
                }
            }
            for(int k=0; k<config.nbShortTermVehicle; k++){
                visit += y[j*config.nbShortTermVehicle + k];
            }
            model.add(visit == 1.0);
        }
        
        // Only use one Short-term vehicle
        for(int k=0; k<config.nbShortTermVehicle; k++){
            IloExpr use(env);
            for(int j=1; j<config.nbVertex; j++){
                use += y[j*config.nbShortTermVehicle + k];
            }
            model.add(use <= 1.0);
        }

        // Only use one vehicle
        for(int k=0; k<config.nbVehicle; k++){
            IloExpr use(env);
            for(int j=1; j<config.nbVertex; j++){
                use += x[j*config.nbVehicle + k];
            }
            model.add(use <= 1.0);
        }
        
        // Subtour elimination constraints
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

        // Subtour elimination variable for base depot
        for(int k=0; k<config.nbVehicle; k++){
            model.add(u[0*config.nbVehicle + k] == 0.0);
        }

        // ------------------------------------- Solve the model -------------------------------------

        IloCplex cplex(model);
        if (verbose <= 1){
            cplex.setOut(env.getNullStream());
        }

        cplex.solve();
        best_score = cplex.getObjValue();
        if (verbose >= 1){
            std::cout << "Best score: " << best_score << std::endl;
        }

        // ------------------------------------- Get the results -------------------------------------
        IloNumArray valsx(env);
        cplex.getValues(x, valsx);
        for(int k = 0; k < config.nbVehicle; k++){
            int current_node = 0;
            do{
                for(int j=0; j<config.nbVertex; j++){
                    if(valsx[current_node*config.nbVehicle*config.nbVertex + j*config.nbVehicle + k] == 1){
                        current_node = j;
                        if (current_node != 0){
                            solution[k].push_back(current_node);
                        }
                        break;
                    }
                }
            }while(current_node != 0);
        }
        IloNumArray valsy(env);
        cplex.getValues(y, valsy);
        for(int k = 0; k < config.nbShortTermVehicle; k++){
            for(int i=0; i<config.nbVertex; i++){
                if(valsy[i*config.nbShortTermVehicle + k] == 1){
                    solution[k+config.nbVehicle].push_back(i);
                    break;
                }
            }
        }
        // ------------------------------------- Display the results -------------------------------------
        if (verbose >= 1){
            if(verbose >= 2){
                // display x
                std::cout << "x :" << std::endl;
                std::cout << valsx << std::endl;
            }
            
            for(int k = 0; k < config.nbVehicle; k++){
                for(int i=0; i<config.nbVertex; i++){
                    for(int j=0; j<config.nbVertex; j++){
                        if(valsx[i*config.nbVehicle*config.nbVertex + j*config.nbVehicle + k] == 1){
                            std::cout << "Vehicle " << k << " travels from " << i << " to " << j << std::endl;
                        }
                    }
                }
            }
        }

        if (verbose >= 1){
            if(verbose >= 2){
                // display y
                std::cout << "y :" << std::endl;
                std::cout << valsy << std::endl;
            }
            for(int k = 0; k < config.nbShortTermVehicle; k++){
                for(int i=0; i<config.nbVertex; i++){
                    if(valsy[i*config.nbShortTermVehicle + k] == 1){
                        std::cout << "Short-term vehicle " << k << " visits customer " << i << std::endl;
                    }
                }
            }
            if(verbose >= 2){
                // display d
                std::cout << "d :" << std::endl;
                cplex.getValues(d, valsy);
                std::cout << valsy << std::endl;

                // display t
                std::cout << "t :" << std::endl;
                cplex.getValues(t, valsy);
                std::cout << valsy << std::endl;
            }
        }

    }
    catch (IloException& ex) {
      std::cerr << "Error: " << ex << std::endl;
   }
   catch (...) {
      std::cerr << "Error" << std::endl;
   }
   env.end();
   return solution;

}
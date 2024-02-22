# OPTIMISATION VRP

## Introduction

## Running the project
### Installation

For Windows : make sure you are using WSL2

Download CPlex from [IBM](https://www.ibm.com/products/ilog-cplex-optimization-studio/cplex-optimizer) and install it.

Modify the `CMakelists.txt` file to add the path to the CPLEX installation.

```cmake
# Path to CPLEX and CONCERT
set(CPLEX_DIR "/opt/ibm/ILOG/CPLEX_Studio2211/cplex")
set(CONCERT_DIR "/opt/ibm/ILOG/CPLEX_Studio2211/concert")
```

### Usage

```bash
bash run.sh <table_number[1:17]> (verbose{0,1,2})
```
The first argument is the number of the table to run. The second argument is the verbosity level. 0 is the default level, 1 is for more details and 2 is for debug.

## Structure of the project

### Importing the data from the tables

The data is stored in the `tables` folder. Each table is decomposed into 3 files: `distance.csv`, `demands.csv` and `vehicles.csv`.
All those files have been made from the original tables using the `cleaner.py` script.

The `config.cpp` file is responsible for importing the data from the tables and storing it in the `Config` class.

### The CPLEX model

The CPLEX model is implemented in the `CPLEX_implex.cpp` file. It is responsible for creating the model from the `Config` class, adding the constraints and the objective function and solving the model. 
The mathematical model is described in the `report.pdf` file.

### The Exact algorithm

The exact algorithm is implemented in the `exact_solver.cpp` file. It is responsible for solving the problem using an exact algorithm. It is a hierarchical approach, decomposing the problem into a partitioning problem and a TSP.
The TSP is solved using the Held-Karp algorithm (dynamic programming in O(n^2*2^n)) and the partitioning problem is solved by brute force (in O(3^n) for 3 vehicles).

### The Bottom-up Heuristic algorithm

The bottom-up heuristic algorithm is implemented in the `heuristic.cpp` file. It is an adaptation of the exact algorithm, to tackle problems with a large number of clients. In place of solving the TSP exactly, we find an upper bound using a MST and shortcutting the tour. The partitioning problem is solved using a greedy algorithm.

### The Local Search Heuristic algorithm

The local search heuristic algorithm is implemented in the `local_search.cpp` file. It is responsible for solving the problem using a local search algorithm. It is a 2-opt algorithm, which is a simple and efficient algorithm to improve the solution of a TSP.

### The `run.sh` script

The `run.sh` script is a bash script that compiles the project and runs it with the given arguments. It calls the `vrp.cpp` file which is the main file of the project, responsible for calling every algorithm and printing the time and the results.

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
bash run.sh <tab_number[1:17]> (verbose{0,1,2})
```
The first argument is the number of the tab to run. The second argument is the verbosity level. 0 is the default level, 1 is for more details and 2 is for debug.

## Structure of the project

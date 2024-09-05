# Introduction

This project aims to implement a multi-threaded version of the PageRank algorithm. The algorithm is used to rank web pages
in search engines, and it is based on the idea that a page is important if it is linked by other important pages. The algorithm is
iterative and computes the PageRank values of the nodes in a graph. See the report for firther details.

# Project Structure

The project is divided into the following files:

1. **main.cpp**: The main file that reads the input graph and calls the PageRank function. It takes arguments for the path to a list of directed graph edges and the maximum number of threads to use. It then measures the execution time from 1 to the maximum number of threads and saves the results in a `.csv` file inside the `stats` folder.

2. **main_perf.cpp**: Another main file specifically created for analysis using the Perf tool. It takes arguments for the path to a list of directed graph edges, the algorithm to use (a or b), and whether it's sequential or parallel (s or p). It simply calls the required PageRank function.

3. **main_scorep.cpp**: Another main file specifically created for analysis using the Score-P tool. It takes arguments for the path to a list of directed graph edges and performs parallel PageRank using all available threads.

4. **utility.h and .cpp**: A file containing utility functions such as the function to read the input graph and the function to calculate statistics.

5. **graph/graph.h and .cpp**: A file containing the graph class that stores a column-wise graph and the functions to calculate PageRank.

6. **graph/graph\_by\_row.h and .cpp**: A file containing the graph class that stores a row-wise graph and the functions to calculate PageRank.

7. **speedup\_graphs.py**: A Python script that reads the `.csv` files inside the `stats` folder and generates speedup graphs.

8. **graph\_generator.py**: A Python script that generates a list of random directed graph edges with a given number of nodes and edges.

# How to Run
## Main File
To compile the project, run the following commands:
```bash
g++ -std=c++20 main.cpp utility.cpp graph/graph.cpp graph/graph_by_row.cpp -o main -fopenmp -O3
```
To run the project, use the following command:
```
./main <path_to_graph_edges> <max_threads>
```

Example (Run main on p2p_Gnutella31 up to 26 threads):
```bash 
./main ./graphs/p2p_Gnutella31.txt 26
```

## Perf Tool
To compile and run the project for analysis using the Perf tool, run the following commands:
```bash
g++ -std=c++20 main_perf.cpp utility.cpp graph/graph.cpp graph/graph_by_row.cpp -o main_perf -fopenmp -O3
```

To run the project for analysis using the Perf tool, use the following command:
```
perf record -g ./main_perf <path_to_graph_edges> <algorithm> <mode>
```

Example (Run Algorithm 1 Parallel on p2p_Gnutella31):
```bash
perf stat -d ./main_perf ./graphs/p2p_Gnutella31.txt a p
```

## Score-P Tool
To compile and run the project for analysis using the Score-P tool (must be installed before), run the following commands:
```bash
mkdir scorep
cd scorep
scorep-g++ -std=c++20 ../main_scorep.cpp ../utility.cpp ../graph/graph.cpp ../graph/graph_by_row.cpp -o main_scorep -fopenmp
```

To run the project for analysis using the Score-P tool, use the following command:
```
mpirun -np 1 ./main_scorep <path_to_graph_edges>
``` 
This will produce a folder named `scorep-<timestamp>` containing the analysis results ready to be visualized using the `Vampir` tool.

Example (Run main_scorep on p2p_Gnutella31):
```bash
mpirun -np 1 ./main_scorep ../graphs/p2p_Gnutella31.txt
```

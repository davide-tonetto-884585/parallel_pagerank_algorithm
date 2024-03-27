#include <iostream>
#include <chrono>
#include <unordered_map>
#include <omp.h>
#include <fstream>
#include "graph/graph.h"
#include "graph/graph_by_row.h"
#include "utility.h"

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename> <max_num_thread_stats (optional)>" << std::endl;
        return 1;
    }

    std::string filename(argv[1]);

    std::cout << "File: " << filename << std::endl;
    std::cout << "Parsing file..." << std::endl;

    std::vector<std::pair<unsigned int, unsigned int>> edges = utility::parse_edges_from_file_and_normalize(filename);
    unsigned int n = 0;
    for (auto &edge: edges) {
        n = std::max(n, std::max(edge.first, edge.second));
    }
    n++;

    std::cout << "File parsed!" << std::endl << std::endl;
    std::cout << "Number of nodes: " << n << std::endl;
    std::cout << "Number of edges: " << edges.size() << std::endl;
    std::cout << "Number of threads available: " << omp_get_max_threads() << std::endl << std::endl;

    auto begin = std::chrono::high_resolution_clock::now();
    graph g(n, edges);
    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Number of dead ends: " << g.get_num_dead_ends() << std::endl;
    std::cout << "Graph creation time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
              << "ms" << std::endl;

    std::cout << std::endl << "Running parallel page rank..." << std::endl;

    begin = std::chrono::high_resolution_clock::now();
    std::vector<float> r_seq = g.par_page_rank(std::vector<float>(n, 1.0 / n), 0.85, 50, 1e-7, -1);
    end = std::chrono::high_resolution_clock::now();

    std::cout << "Parallel time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
              << "ms"
              << std::endl;

    return 0;
}
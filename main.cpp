#include <iostream>
#include <chrono>
#include <unordered_map>
#include <omp.h>
#include <fstream>
#include <numeric>
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
    // g.print(10);

    std::cout << "Number of dead ends: " << g.get_num_dead_ends() << std::endl;
    std::cout << "Density: " << g.get_density() << std::endl;
    std::cout << "Graph creation time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
              << "ms" << std::endl;

    std::cout << std::endl << "Running sequential and parallel page rank..." << std::endl;

    begin = std::chrono::high_resolution_clock::now();
    std::vector<float> r_seq = g.seq_page_rank(std::vector<float>(n, 1.0 / n), 0.85, 50, 1e-7);
    end = std::chrono::high_resolution_clock::now();

    std::cout << "Sequential time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
              << "ms"
              << std::endl;

    begin = std::chrono::high_resolution_clock::now();
    std::vector<float> r_par = g.par_page_rank(std::vector<float>(n, 1.0 / n), 0.85, 50, 1e-7, -1);
    end = std::chrono::high_resolution_clock::now();

    std::cout << "Parallel time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms"
              << std::endl;

    // compare results
    if (!utility::compare_vectors(r_seq, r_par)) {
        std::cerr << "Results are different!" << std::endl;
        return 1;
    }

    std::cout << "Results are equal!" << std::endl;

    int max_n_threads = argc > 2 ? std::stoi(argv[2]) : -1;
    std::cout << std::endl << "Computing speedup up to " << max_n_threads << " threads..." << std::endl;

    auto stats = utility::get_stats_pagerank(g, max_n_threads);

    // Create file and save speedup
    std::string filename_no_ext = filename.substr(filename.find_last_of('/') + 1,
                                                  filename.find_last_of('.') - filename.find_last_of('/') - 1);
    std::ofstream file{"../stats/pagerank_speedup_" + filename_no_ext + ".csv"};

    std::cout << "Speedup computed! Saving to file " << "../stats/pagerank_speedup_" + filename_no_ext + ".csv"
              << std::endl;

    file << "n_thread,milliseconds" << std::endl;
    for (auto &p: stats["pagerank_speedup"]) {
        file << p.first << "," << p.second << std::endl;
    }
    file.close();

    std::cout << "File saved!" << std::endl;

    std::cout << std::endl;

    begin = std::chrono::high_resolution_clock::now();
    graph_by_row g_by_row(n, edges);
    end = std::chrono::high_resolution_clock::now();

    std::cout << "Graph version 2 creation time: "
              << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
              << "ms" << std::endl;

    begin = std::chrono::high_resolution_clock::now();
    std::vector<float> r_seq_v2 = g_by_row.seq_page_rank(std::vector<float>(n, 1.0 / n), 0.85, 50, 1e-7);
    end = std::chrono::high_resolution_clock::now();

    std::cout << "Sequential time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count()
              << "ms"
              << std::endl;

    // compare results
    if (!utility::compare_vectors(r_seq_v2, r_seq)) {
        std::cerr << "Results are different!" << std::endl;
        return 1;
    }

    begin = std::chrono::high_resolution_clock::now();
    std::vector<float> r_par_v2 = g_by_row.par_page_rank(std::vector<float>(n, 1.0 / n), 0.85, 50, 1e-7, -1);
    end = std::chrono::high_resolution_clock::now();

    std::cout << "Parallel time: " << std::chrono::duration_cast<std::chrono::milliseconds>(end - begin).count() << "ms"
              << std::endl;

    // compare results
    if (!utility::compare_vectors(r_par_v2, r_par)) {
        std::cerr << "Results are different!" << std::endl;
        return 1;
    }

    std::cout << "Results are equal!" << std::endl;

    std::cout << std::endl << "Computing speedup up to " << max_n_threads << " threads..." << std::endl;

    stats = utility::get_stats_pagerank(g_by_row, max_n_threads);

    // Create file and save speedup
    filename_no_ext = filename.substr(filename.find_last_of('/') + 1,
                                      filename.find_last_of('.') - filename.find_last_of('/') - 1);
    file.open("../stats/pagerank_speedup_" + filename_no_ext + "_v2.csv");

    std::cout << "Speedup computed! Saving to file " << "../stats/pagerank_speedup_" + filename_no_ext + ".csv"
              << std::endl;

    file << "n_thread,milliseconds" << std::endl;
    for (auto &p: stats["pagerank_speedup"]) {
        file << p.first << "," << p.second << std::endl;
    }
    file.close();

    std::cout << "File saved!" << std::endl;

    return 0;
}

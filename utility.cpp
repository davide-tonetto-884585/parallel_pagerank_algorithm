#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <sstream>
#include <iostream>
#include <numeric>
#include <cmath>
#include <chrono>
#include <omp.h>
#include "graph/graph.h"
#include "graph/graph_by_row.h"
#include "utility.h"

namespace utility {
    std::vector <std::pair<unsigned int, unsigned int>>
    parse_edges_from_file_and_normalize(const std::string &filename) {
        std::unordered_map<unsigned int, unsigned int> map;
        std::vector <std::pair<unsigned int, unsigned int>> edges;
        std::ifstream file(filename);
        unsigned int last_id = 0, u, v;
        std::string line;
        if (file.is_open()) {
            while (!file.eof()) {
                // read line and check if it contains two numbers
                std::getline(file, line);
                if (line.empty())
                    continue;

                std::stringstream sline(line);
                sline >> u >> v;
                if (sline.fail())
                    continue;

                if (!map.contains(u))
                    map[u] = last_id++;

                if (!map.contains(v))
                    map[v] = last_id++;

                edges.emplace_back(map[u], map[v]);
            }
        } else {
            std::cerr << "Error: cannot open file " << filename << std::endl;
            exit(1);
        }

        return edges;
    }

    double round(double x, unsigned int d) {
        return std::round(x * std::pow(10, d)) / std::pow(10, d);
    }

    bool check_distribution(const std::vector<float> &r) {
        return round(std::accumulate(r.begin(), r.end(), 0.0), 3) == 1;
    }

    bool compare_vectors(const std::vector<float> &v1, const std::vector<float> &v2, double tolerance) {
        if (v1.size() != v2.size())
            return false;

        for (unsigned int i = 0; i < v1.size(); i++) {
            if (std::abs(v1[i] - v2[i]) > tolerance) {
                std::cout << "Difference at index " << i << ": " << v1[i] << " != " << v2[i] << std::endl;
                return false;
            }
        }

        return true;
    }

    std::unordered_map<std::string, std::vector<std::pair<double, double>>> get_stats_pagerank(const graph &g,
                                                                                               int max_n_threads) {
        // compute speedup
        std::unordered_map<std::string, std::vector<std::pair<double, double>>> stats;
        std::vector<float> v(g.get_n(), static_cast<float>(1) / g.get_n());
        unsigned int n_threads = max_n_threads == -1 ? omp_get_max_threads() : max_n_threads;
        for (unsigned int i = 1; i <= n_threads; i++) {
            if (i == 1) {
                auto begin = std::chrono::high_resolution_clock::now();
                g.seq_page_rank(v, 0.85, 50, 1e-7);
                auto end = std::chrono::high_resolution_clock::now();
                stats["pagerank_speedup"].emplace_back(i,
                                                       std::chrono::duration_cast<std::chrono::milliseconds>(
                                                               end - begin).count());
            } else {
                auto begin = std::chrono::high_resolution_clock::now();
                g.par_page_rank(v, 0.85, 50, 1e-7, i);
                auto end = std::chrono::high_resolution_clock::now();
                stats["pagerank_speedup"].emplace_back(i,
                                                       std::chrono::duration_cast<std::chrono::milliseconds>(
                                                               end - begin).count());
            }
        }

        return stats;
    }

    std::unordered_map<std::string, std::vector<std::pair<double, double>>> get_stats_pagerank(const graph_by_row &g,
                                                                                               int max_n_threads) {
        // compute speedup
        std::unordered_map<std::string, std::vector<std::pair<double, double>>> stats;
        std::vector<float> v(g.get_n(), static_cast<float>(1) / g.get_n());
        unsigned int n_threads = max_n_threads == -1 ? omp_get_max_threads() : max_n_threads;
        for (unsigned int i = 1; i <= n_threads; i++) {
            if (i == 1) {
                auto begin = std::chrono::high_resolution_clock::now();
                g.seq_page_rank(v, 0.85, 50, 1e-7);
                auto end = std::chrono::high_resolution_clock::now();
                stats["pagerank_speedup"].emplace_back(i,
                                                       std::chrono::duration_cast<std::chrono::milliseconds>(
                                                               end - begin).count());
            } else {
                auto begin = std::chrono::high_resolution_clock::now();
                g.par_page_rank(v, 0.85, 50, 1e-7, i);
                auto end = std::chrono::high_resolution_clock::now();
                stats["pagerank_speedup"].emplace_back(i,
                                                       std::chrono::duration_cast<std::chrono::milliseconds>(
                                                               end - begin).count());
            }
        }

        return stats;
    }
}

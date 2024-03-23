//
// Created by davide on 21/03/24.
//

#ifndef ASSIGNMENT_1_LMD_UTILITY_H
#define ASSIGNMENT_1_LMD_UTILITY_H

#include <unordered_map>
#include "graph/graph.h"
#include "graph/graph_by_row.h"

namespace utility {
    std::vector <std::pair<unsigned int, unsigned int>>
    parse_edges_from_file_and_normalize(const std::string &filename);

    double round(double x, unsigned int d);

    bool check_distribution(const std::vector<float> &r);

    bool compare_vectors(const std::vector<float> &v1, const std::vector<float> &v2, double tolerance);

    std::unordered_map<std::string, std::vector<std::pair<double, double>>> get_stats_pagerank(const graph &g, int max_n_threads);

    std::unordered_map<std::string, std::vector<std::pair<double, double>>> get_stats_pagerank(const graph_by_row &g, int max_n_threads);
}

#endif //ASSIGNMENT_1_LMD_UTILITY_H

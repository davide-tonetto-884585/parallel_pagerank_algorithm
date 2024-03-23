#include <iostream>
#include <string>
#include <vector>
#include "utility.h"
#include "graph/graph.h"
#include "graph/graph_by_row.h"

int main(int argc, char *argv[]) {
    if (argc < 4) {
        std::cerr << "Usage: " << argv[0] << " <filename> <graph_type (a or b)> <algorithm_type (p or s)>" << std::endl;
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

    if (argv[2][0] == 'a') {
        graph g(n, edges);
        if (argv[3][0] == 'p') {
            g.par_page_rank(std::vector<float>(n, static_cast<float>(1) / n), 0.85, 50, 1e-7, -1);
        } else {
            g.seq_page_rank(std::vector<float>(n, static_cast<float>(1) / n), 0.85, 50, 1e-7);
        }
    } else {
        graph_by_row gbr(n, edges);
        if (argv[3][0] == 'p') {
            gbr.par_page_rank(std::vector<float>(n, static_cast<float>(1) / n), 0.85, 50, 1e-7, -1);
        } else {
            gbr.seq_page_rank(std::vector<float>(n, static_cast<float>(1) / n), 0.85, 50, 1e-7);
        }
    }
}
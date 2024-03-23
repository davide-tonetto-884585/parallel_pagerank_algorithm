#include <iostream>
#include <string>
#include <vector>
#include "utility.h"
#include "graph/graph.h"
#include "graph/graph_by_row.h"

graph *create_graph(const std::string &filename) {
    std::vector<std::pair<unsigned int, unsigned int>> edges = utility::parse_edges_from_file_and_normalize(filename);
    unsigned int n = 0;
    for (auto &edge: edges) {
        n = std::max(n, std::max(edge.first, edge.second));
    }
    n++;

    return new graph(n, edges);
}

graph_by_row *create_graph_v2(const std::string &filename) {
    std::vector<std::pair<unsigned int, unsigned int>> edges = utility::parse_edges_from_file_and_normalize(filename);
    unsigned int n = 0;
    for (auto &edge: edges) {
        n = std::max(n, std::max(edge.first, edge.second));
    }
    n++;

    return new graph_by_row(n, edges);
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return 1;
    }

    std::string filename(argv[1]);
    std::cout << "File: " << filename << std::endl;
    std::cout << "Creating graph v1..." << std::endl;

    graph *g1 = create_graph(filename);
    delete g1;

    std::cout << "Creating graph v2..." << std::endl;
    graph_by_row *g2 = create_graph_v2(filename);
    delete g2;
}
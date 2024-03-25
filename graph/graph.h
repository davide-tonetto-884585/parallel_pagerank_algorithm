#ifndef ASSIGNMENT_1_LMD_GRAPH_H
#define ASSIGNMENT_1_LMD_GRAPH_H

// for each column of M, store non-zero elements by using
// an array of node ids, and a single value for o(j)
class graph {
private:
    size_t n; // number of nodes
    size_t m; // number of edges

    // sparse graph representation for adjacency graph
    std::vector<unsigned int> **col_ids; // array of node ids

public:
    graph(const unsigned int &n, const std::vector<std::pair<unsigned int, unsigned int>> &edges);

    ~graph();

    // operators
    bool operator==(const graph &g) const;

    // return the number of nodes
    unsigned int get_n() const;

    unsigned int get_m() const;

    double get_density() const;

    unsigned int get_num_dead_ends() const;

    void print(unsigned int max_n) const;

    std::vector<float>
    seq_page_rank(const std::vector<float> &v, float beta, unsigned int max_iterations, double tolerance) const;

    std::vector<float>
    par_page_rank(const std::vector<float> &v, float beta, unsigned int max_iterations, double tolerance,
                  int n_thread = -1) const;
};


#endif //ASSIGNMENT_1_LMD_GRAPH_H

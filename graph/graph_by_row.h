#ifndef ASSIGNMENT_1_LMD_GRAPH_BY_ROW_H
#define ASSIGNMENT_1_LMD_GRAPH_BY_ROW_H

class graph_by_row {
private:
    size_t n; // number of nodesdouble

    // sparse graph representation for adjacency graph
    std::vector<unsigned int> **row_ids; // array of node ids
    std::vector<unsigned int> count_col_elements;
    std::vector<unsigned int> *dead_ends_ids;

public:
    graph_by_row(const unsigned int &n, const std::vector<std::pair<unsigned int, unsigned int>> &edges);

    ~graph_by_row();

    // return the number of nodes
    unsigned int get_n() const;

    unsigned int get_num_dead_ends() const;

    void print() const;

    std::vector<float>
    seq_page_rank(const std::vector<float> &v, float beta, unsigned int max_iterations, double tolerance) const;

    std::vector<float>
    par_page_rank(const std::vector<float> &v, float beta, unsigned int max_iterations, double tolerance,
                  int n_thread = -1) const;
};

#endif //ASSIGNMENT_1_LMD_GRAPH_BY_ROW_H

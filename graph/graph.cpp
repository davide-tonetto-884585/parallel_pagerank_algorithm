#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <complex>
#include <omp.h>
#include <set>
#include "graph.h"
#include "../utility.h"

#pragma omp declare reduction (merge : std::vector<int> : omp_out.insert(omp_out.end(), omp_in.begin(), omp_in.end()))

graph::graph(const unsigned int &n, const std::vector<std::pair<unsigned int, unsigned int>> &edges)
        : n(n) {
    col_ids = new std::vector<unsigned int> *[n]{nullptr};

    for (auto &edge: edges) {
        if (col_ids[edge.first] == nullptr)
            col_ids[edge.first] = new std::vector<unsigned int>;

        col_ids[edge.first]->push_back(edge.second);
    }

    for (unsigned int i = 0; i < n; i++) {
        if (col_ids[i] != nullptr) {
            col_ids[i]->shrink_to_fit();
        }
    }
}

graph::~graph() {
    for (unsigned int i = 0; i < n; i++) {
        if (col_ids[i] != nullptr)
            delete col_ids[i];
    }
    delete[] col_ids;
}

bool graph::operator==(const graph &g) const {
    if (n != g.n)
        return false;

    for (unsigned int i = 0; i < n; i++) {
        if (col_ids[i] == nullptr && g.col_ids[i] != nullptr)
            return false;

        if (col_ids[i] != nullptr && g.col_ids[i] == nullptr)
            return false;

        if (col_ids[i] != nullptr && g.col_ids[i] != nullptr) {
            if (col_ids[i]->size() != g.col_ids[i]->size())
                return false;

            for (unsigned int j = 0; j < col_ids[i]->size(); j++) {
                // non ordered comparison
                if (std::find(g.col_ids[i]->begin(), g.col_ids[i]->end(), col_ids[i]->at(j)) == g.col_ids[i]->end())
                    return false;
            }
        }
    }

    return true;
}

unsigned int graph::get_n() const {
    return n;
}

unsigned int graph::get_num_dead_ends() const {
    unsigned int num_dead_ends = 0;
    for (unsigned int i = 0; i < n; i++) {
        if (col_ids[i] == nullptr)
            num_dead_ends++;
    }

    return num_dead_ends;
}

void graph::print() const {
    std::cout << "Adjacency graph:" << std::endl;
    for (unsigned int i = 0; i < n; i++) {
        for (unsigned int j = 0; j < n; j++) {
            std::cout << (std::find(col_ids[j]->begin(), col_ids[j]->end(), i) != col_ids[j]->end() ? 1. /
                                                                                                      (float) col_ids[j]->size()
                                                                                                    : 0) << " ";
        }

        std::cout << std::endl;
    }

    std::cout << std::endl;
}

std::vector<float>
graph::seq_page_rank(const std::vector<float> &v, float beta, unsigned int max_iterations, double tolerance) const {
    std::vector<float> r(v), r_new(v);
    unsigned int iterations = 0;
    float sum, teleportation_correction = (1 - beta) / static_cast<float>(n);


    do {
        r = r_new;
        std::fill(r_new.begin(), r_new.end(), 0.0);
        float r_sum_dead_ends = 0;
        for (unsigned int i = 0; i < n; i++) {
            if (col_ids[i] == nullptr)
                r_sum_dead_ends += r[i];
            else {
                float r_i_divided = r[i] / static_cast<float>(col_ids[i]->size());
                for (auto &j: *col_ids[i]) {
                    r_new[j] += r_i_divided;
                }
            }
        }

        // apply teleportation and add all dead ends to each node
        sum = 0;
        float dead_end_weight = r_sum_dead_ends / static_cast<float>(n);
        for (unsigned int i = 0; i < n; i++) {
            r_new[i] = (r_new[i] + dead_end_weight) * beta + teleportation_correction;
            sum += static_cast<float>(std::pow(r_new[i] - r[i], 2));
        }
    } while (++iterations < max_iterations && std::sqrt(sum) > tolerance);

    // std::cout << "Iterations: " << iterations << std::endl;

    if (!utility::check_distribution(r_new)) {
        std::cerr << "The distribution is not correct! The vector sum up to "
                  << std::accumulate(r_new.begin(), r_new.end(), 0.0) << std::endl;
    }

    return r_new;
}

std::vector<float>
graph::par_page_rank(const std::vector<float> &v, float beta, unsigned int max_iterations, double tolerance,
                     int n_thread) const {
    std::vector<float> r(v), r_new(v);
    unsigned int iterations = 0;
    float sum;

    // If n_thread is -1, use all available threads
    if (n_thread == -1) {
        n_thread = omp_get_max_threads();
    }

#pragma omp ordered
    do {
        r = r_new;
        std::fill(r_new.begin(), r_new.end(), 0.0);
        float r_sum_dead_ends = 0;

#pragma omp parallel for if(n_thread != 1) num_threads(n_thread) default(none) \
        shared(r, r_new, col_ids, n) \
        reduction(+:r_sum_dead_ends)

        for (unsigned int i = 0; i < n; i++) {
            if (col_ids[i] == nullptr)
                r_sum_dead_ends += r[i];
            else {
                float r_i_divided = r[i] / static_cast<float>(col_ids[i]->size());
                for (auto &j: *col_ids[i]) {
#pragma omp atomic update
                    r_new[j] += r_i_divided;
                }
            }
        }

        // apply teleportation and add all dead ends to each node
        sum = 0;
        float teleportation_correction = (1 - beta) / static_cast<float>(n);
        float dead_end_weight = r_sum_dead_ends / static_cast<float>(n);

#pragma omp parallel for if(n_thread != 1) num_threads(n_thread) default(none) \
        firstprivate(beta, dead_end_weight, teleportation_correction) shared(r, r_new, n) \
        reduction(+:sum)

        for (unsigned int i = 0; i < n; i++) {
            r_new[i] = (r_new[i] + dead_end_weight) * beta + teleportation_correction;
            sum += static_cast<float>(std::pow(r_new[i] - r[i], 2));
        }
    } while (++iterations < max_iterations && std::sqrt(sum) > tolerance);

    // std::cout << "Iterations: " << iterations << std::endl;

    if (!utility::check_distribution(r_new)) {
        std::cerr << "The distribution is not correct! The vector sum up to "
                  << std::accumulate(r_new.begin(), r_new.end(), 0.0) << std::endl;
    }

    return r_new;
}

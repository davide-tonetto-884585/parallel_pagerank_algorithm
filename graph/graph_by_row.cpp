#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <complex>
#include <omp.h>
#include <set>
#include "graph_by_row.h"
#include "../utility.h"

graph_by_row::graph_by_row(const unsigned int &n, const std::vector<std::pair<unsigned int, unsigned int>> &edges) :
        n(n), m(edges.size()), dead_ends_ids(nullptr) {
    row_ids = new std::vector<unsigned int> *[n]{nullptr};
    count_col_elements = std::vector<unsigned int>(n, 0);

    for (auto &edge: edges) {
        if (row_ids[edge.second] == nullptr)
            row_ids[edge.second] = new std::vector<unsigned int>;

        row_ids[edge.second]->push_back(edge.first);
        count_col_elements[edge.first]++;
    }

    for (unsigned int i = 0; i < n; ++i) {
        if (row_ids[i] != nullptr)
            row_ids[i]->shrink_to_fit();
        if (count_col_elements[i] == 0) {
            if (dead_ends_ids == nullptr) {
                dead_ends_ids = new std::vector<unsigned int>;
            }

            dead_ends_ids->push_back(i);
        }
    }
}

graph_by_row::~graph_by_row() {
    for (unsigned int i = 0; i < n; ++i) {
        if (row_ids[i] != nullptr) {
            delete row_ids[i];
        }
    }

    delete[] row_ids;
    delete dead_ends_ids;
}

unsigned int graph_by_row::get_n() const {
    return n;
}

unsigned int graph_by_row::get_num_dead_ends() const {
    return dead_ends_ids == nullptr ? 0 : dead_ends_ids->size();
}

void graph_by_row::print() const {
    for (unsigned int i = 0; i < n; ++i) {
        if (row_ids[i] != nullptr) {
            std::cout << i << " -> ";
            for (auto &id: *row_ids[i]) {
                std::cout << id << " ";
            }
            std::cout << std::endl;
        }
    }
}

std::vector<float> graph_by_row::seq_page_rank(const std::vector<float> &v, float beta, unsigned int max_iterations,
                                               double tolerance) const {
    std::vector<float> r(v), r_new(v);
    unsigned int iterations = 0;
    float sum, teleportation_correction = (1 - beta) / static_cast<float>(n);

    do {
        r = r_new;
        std::fill(r_new.begin(), r_new.end(), 0.0);
        sum = 0;

        for (unsigned int i = 0; i < n; ++i) {
            if (row_ids[i] != nullptr) {
                for (auto &j: *row_ids[i]) {
                    r_new[i] += r[j] / static_cast<float>(count_col_elements[j]);
                }
            }

            // add dead ends
            if (dead_ends_ids != nullptr) {
                for (auto &j: *dead_ends_ids) {
                    r_new[i] += r[j] / static_cast<float>(n);
                }
            }

            // apply teleportation
            r_new[i] = r_new[i] * beta + teleportation_correction;
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
graph_by_row::par_page_rank(const std::vector<float> &v, float beta, unsigned int max_iterations, double tolerance,
                            int n_thread) const {
    std::vector<float> r(v), r_new(v);
    unsigned int iterations = 0;
    float sum, teleportation_correction = (1 - beta) / static_cast<float>(n);

    // If n_thread is -1, use all available threads
    if (n_thread == -1) {
        n_thread = omp_get_max_threads();
    }

#pragma omp ordered
    do {
        r = r_new;
        std::fill(r_new.begin(), r_new.end(), 0.0);
        sum = 0;

#pragma omp parallel for if(n_thread != 1) num_threads(n_thread) default(none) \
    firstprivate(teleportation_correction, beta, n, m, n_thread) shared(r, r_new, count_col_elements, row_ids, dead_ends_ids) \
    schedule(dynamic, n_thread) \
    reduction(+:sum)
        for (unsigned int i = 0; i < n; ++i) {
            if (row_ids[i] != nullptr) {
                for (auto &j: *row_ids[i]) {
                    r_new[i] += r[j] / static_cast<float>(count_col_elements[j]);
                }
            }

            // add dead ends
            if (dead_ends_ids != nullptr) {
                for (auto &j: *dead_ends_ids) {
                    r_new[i] += r[j] / static_cast<float>(n);
                }
            }

            // apply teleportation
            r_new[i] = r_new[i] * beta + teleportation_correction;
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

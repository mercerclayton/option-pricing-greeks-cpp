/*
Level 9: Introductory Computational Finance
Group A & B: Exact Pricing Methods

Implementation of the Mesh function.
*/
#include <cmath>
#include <stdexcept>

#include "Mesh.hpp"

std::vector<double> Mesh(double start, double end, double h) {

    if (!(h > 0)) {
        throw std::invalid_argument("Mesh: step size h must be > 0.");
    }
    if (!(end >= start)) {
        throw std::invalid_argument("Mesh: end must be >= start.");
    }

    // Number of steps. The small tolerance absorbs rounding errors.
    const auto steps =
        static_cast<std::size_t>(std::floor((end - start) / h + 1e-9));

    std::vector<double> mesh;
    mesh.reserve(steps + 1);
    for (std::size_t i = 0; i <= steps; ++i) {
        mesh.push_back(start + i * h);
    }
    return mesh;
}

/*
Level 9: Introductory Computational Finance
Group A & B: Exact Pricing Methods

Applies any calculation (price, greek, ...) to a vector or matrix of option
parameter sets.
*/
#ifndef MATRIX_PRICER_HPP
#define MATRIX_PRICER_HPP

#include <vector>

#include "OptionData.hpp"

typedef std::vector<std::vector<OptionParameters>> ParameterMatrix;
typedef std::vector<std::vector<double>> Matrix;

/*
Applies f to each parameter set in a vector and returns the results.
F is any callable taking (const OptionParameters&) and returning double.
*/
template <typename F>
std::vector<double> Compute(const std::vector<OptionParameters>& params, F f) {
    std::vector<double> result;
    result.reserve(params.size());
    for (const auto& p : params) {
        result.push_back(f(p));
    }
    return result;
}

/*
Applies f to each parameter set in a matrix and returns a matrix of results
with the same shape.
*/
template <typename F>
Matrix Compute(const ParameterMatrix& params, F f) {
    Matrix result;
    result.reserve(params.size());
    for (const auto& row : params) {
        result.push_back(Compute(row, f));
    }
    return result;
}

/*
Builds a matrix of parameter sets from base: row i has rowField set to
rowValues[i], column j has colField set to colValues[j].
Example: ParameterGrid(params, &OptionParameters::T, {0.25, 0.5},
                               &OptionParameters::S, Mesh(55, 75, 5))
*/
inline ParameterMatrix ParameterGrid(const OptionParameters& base,
                                     double OptionParameters::* rowField,
                                     const std::vector<double>& rowValues,
                                     double OptionParameters::* colField,
                                     const std::vector<double>& colValues) {
    ParameterMatrix grid;
    grid.reserve(rowValues.size());
    for (const auto& rowBase : VaryParameter(base, rowField, rowValues)) {
        grid.push_back(VaryParameter(rowBase, colField, colValues));
    }
    return grid;
}

/*
Delta by central divided difference: [V(S+h) - V(S-h)] / 2h.
price is any callable taking (const OptionParameters&) and returning the
option value, e.g. a lambda calling EuropeanOption{p, type}.Price().
Error is O(h^2), until round-off dominates for very small h.
*/
template <typename F>
double DeltaFD(F price, const OptionParameters& p, double h) {
    if (!(h > 0)) {
        throw std::invalid_argument("DeltaFD: step size h must be > 0.");
    }
    return (price(p.With(&OptionParameters::S, p.S + h)) -
            price(p.With(&OptionParameters::S, p.S - h))) /
           (2.0 * h);
}

/*
Gamma by central divided difference: [V(S+h) - 2V(S) + V(S-h)] / h^2.
*/
template <typename F>
double GammaFD(F price, const OptionParameters& p, double h) {
    if (!(h > 0)) {
        throw std::invalid_argument("GammaFD: step size h must be > 0.");
    }
    return (price(p.With(&OptionParameters::S, p.S + h)) - 2.0 * price(p) +
            price(p.With(&OptionParameters::S, p.S - h))) /
           (h * h);
}

#endif

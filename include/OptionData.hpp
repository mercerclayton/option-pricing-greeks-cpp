/*
Level 9: Introductory Computational Finance
Group A & B: Exact Pricing Methods
*/
#ifndef OPTION_DATA_HPP
#define OPTION_DATA_HPP

#include <iostream>
#include <stdexcept>
#include <vector>

/*
Parameters required when pricing an option using the generalized Black-Scholes
model.
*/
struct OptionParameters {
    double T; // Expiry time (maturity) in years, e.g. T = 1.0 is one year; must
              // be > 0
    double K; // Strike price of the option; must be > 0
    double sig; // Constant volatility of the underlying, annualized (e.g. 0.30
                // = 30%); must be > 0
    double r; // Risk-free interest rate, annualized and continuously compounded
              // (e.g. 0.08 = 8%)
    double S; // Current price of the underlying asset at which the option is
              // priced; must be > 0
    double b; // Cost of carry, which selects the model:
              //   b = r       Black-Scholes stock option model
              //   b = r - q   Merton model with continuous dividend yield q
              //   b = 0       Black-Scholes futures option model
              //   b = r - R   Garman-Kohlhagen currency model, R = foreign
              //   risk-free rate

    /*
    Constructor. Enforces initialization of all parameters with valid values.
    */
    OptionParameters(double T, double K, double sig, double r, double S,
                     double b)
        : T{T}, K{K}, sig{sig}, r{r}, S{S}, b{b} {
        Validate();
    }

    /*
    Throws std::invalid_argument if any parameter is out of range. Covers param
    = NaN cases.
    */
    void Validate() const {

        if (!(T > 0)) {
            throw std::invalid_argument("Expiry time (maturity) must be > 0.");
        }

        if (!(K > 0)) {
            throw std::invalid_argument("Strike price must be > 0.");
        }

        if (!(sig > 0)) {
            throw std::invalid_argument("Volatility must be > 0.");
        }

        if (!(S > 0)) {
            throw std::invalid_argument(
                "Current price of the underlying must be > 0.");
        }
    }

    /*
    Returns a copy with one parameter replaced; the copy is re-validated.
    Example: params.With(&OptionParameters::sig, 0.25)
    */
    OptionParameters With(double OptionParameters::* field,
                          double value) const {
        OptionParameters copy{*this};
        copy.*field = value;
        copy.Validate();
        return copy;
    }
};

/*
Displays the list of parameters and their values.
*/
inline std::ostream& operator<<(std::ostream& os,
                                const OptionParameters& params) {
    os << "(T=" << params.T << ", K=" << params.K << ", sig=" << params.sig
       << ", r=" << params.r << ", S=" << params.S << ", b=" << params.b << ")";
    return os;
}

/*
Returns one parameter set per value: each is a copy of base with the given
field replaced by that value.
Example: VaryParameter(params, &OptionParameters::S, Mesh(10, 50, 1))
*/
inline std::vector<OptionParameters>
VaryParameter(const OptionParameters& base, double OptionParameters::* field,
              const std::vector<double>& values) {
    std::vector<OptionParameters> result;
    result.reserve(values.size());
    for (double value : values) {
        result.push_back(base.With(field, value));
    }
    return result;
}

/*
Represents the type of Option.
*/
enum class OptionType { Call, Put };

#endif

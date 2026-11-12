/*
Level 9: Introductory Computational Finance
Group A & B: Exact Pricing Methods

Implementation details of an European Option.
*/
#include <boost/math/distributions/normal.hpp>
#include <cmath>
#include <stdexcept>

#include "EuropeanOption.hpp"

namespace {

// Standard normal distribution
const boost::math::normal_distribution<double> N(0.0, 1.0);

}

/*
Constructor which requires option pricing parameters.
*/
EuropeanOption::EuropeanOption(const OptionParameters& params, OptionType type)
    : Option(params, type) {}

/*
Calculates d1
*/
double EuropeanOption::D1() const {
    return (std::log(m_params.S / m_params.K) +
            (m_params.b + (m_params.sig * m_params.sig) / 2) * m_params.T) /
           (m_params.sig * std::sqrt(m_params.T));
}

/*
Calculates d2
*/
double EuropeanOption::D2() const {
    return D1() - m_params.sig * std::sqrt(m_params.T);
}

/*
Cost-of-carry factor e^{(b-r)T}.
*/
double EuropeanOption::Carry() const {
    return std::exp((m_params.b - m_params.r) * m_params.T);
}

/*
Discount factor e^{-rT}.
*/
double EuropeanOption::Discount() const {
    return std::exp(-m_params.r * m_params.T);
}

/*
Prices the option.
*/
double EuropeanOption::Price() const {

    const double d1 = D1();
    const double d2 = D2();

    const double carry = Carry();
    const double discount = Discount();

    switch (m_type) {
    case OptionType::Call:
        return m_params.S * carry * boost::math::cdf(N, d1) -
               m_params.K * discount * boost::math::cdf(N, d2);
    case OptionType::Put:
        return m_params.K * discount * boost::math::cdf(N, -d2) -
               m_params.S * carry * boost::math::cdf(N, -d1);
    default:
        throw std::logic_error("EuropeanOption::Price: unknown OptionType.");
    }
}

/*
Right-hand side of put-call parity: C - P = Se^{(b-r)T} - Ke^{-rT}.
*/
double EuropeanOption::ParityGap(const OptionParameters& params) {
    return params.S * std::exp((params.b - params.r) * params.T) -
           params.K * std::exp(-params.r * params.T);
}

/*
Price of the opposite option type, computed using put-call parity.
*/
double EuropeanOption::ParityPrice() const {
    switch (m_type) {
    case OptionType::Call:
        return Price() - ParityGap(m_params); // P = C - gap
    case OptionType::Put:
        return Price() + ParityGap(m_params); // C = P + gap
    default:
        throw std::logic_error(
            "EuropeanOption::ParityPrice: unknown OptionType.");
    }
}

/*
Checks whether the given call and put prices satisfy put-call parity.
*/
bool EuropeanOption::SatisfiesParity(double call, double put,
                                     const OptionParameters& p, double tol) {
    return std::abs((call - put) - ParityGap(p)) <= tol;
}

/*
Delta: dV/dS.
*/
double EuropeanOption::Delta() const {
    const double carry = Carry();
    const double Nd1 = boost::math::cdf(N, D1());

    switch (m_type) {
    case OptionType::Call:
        return carry * Nd1;
    case OptionType::Put:
        return carry * (Nd1 - 1.0);
    default:
        throw std::logic_error("EuropeanOption::Delta: unknown OptionType.");
    }
}

/*
Gamma: d2V/dS2. Same for calls and puts.
*/
double EuropeanOption::Gamma() const {
    return Carry() * boost::math::pdf(N, D1()) /
           (m_params.S * m_params.sig * std::sqrt(m_params.T));
}

/*
Vega: dV/dsig. Same for calls and puts.
*/
double EuropeanOption::Vega() const {
    return m_params.S * std::sqrt(m_params.T) * Carry() *
           boost::math::pdf(N, D1());
}

/*
Theta: -dV/dT.
*/
double EuropeanOption::Theta() const {
    const double d1 = D1();
    const double d2 = D2();
    const double carry = Carry();
    const double discount = Discount();

    // Time-decay term shared by calls and puts
    const double decay = -m_params.S * m_params.sig * carry *
                         boost::math::pdf(N, d1) /
                         (2.0 * std::sqrt(m_params.T));

    switch (m_type) {
    case OptionType::Call:
        return decay -
               (m_params.b - m_params.r) * m_params.S * carry *
                   boost::math::cdf(N, d1) -
               m_params.r * m_params.K * discount * boost::math::cdf(N, d2);
    case OptionType::Put:
        return decay +
               (m_params.b - m_params.r) * m_params.S * carry *
                   boost::math::cdf(N, -d1) +
               m_params.r * m_params.K * discount * boost::math::cdf(N, -d2);
    default:
        throw std::logic_error("EuropeanOption::Theta: unknown OptionType.");
    }
}

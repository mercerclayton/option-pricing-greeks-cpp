/*
Level 9: Introductory Computational Finance
Group A & B: Exact Pricing Methods

Implementation details of a perpetual American option.
*/
#include <cmath>
#include <stdexcept>

#include "PerpetualAmericanOption.hpp"

/*
Constructor which requires option pricing parameters.
*/
PerpetualAmericanOption::PerpetualAmericanOption(const OptionParameters& params,
                                                 OptionType type)
    : Option(params, type) {}

/*
Calculates y1 = 1/2 - b/sig^2 + sqrt((b/sig^2 - 1/2)^2 + 2r/sig^2).
*/
double PerpetualAmericanOption::Y1() const {
    const double sig2 = m_params.sig * m_params.sig;
    const double a = m_params.b / sig2 - 0.5;
    return 0.5 - m_params.b / sig2 + std::sqrt(a * a + 2.0 * m_params.r / sig2);
}

/*
Calculates y2 = 1/2 - b/sig^2 - sqrt((b/sig^2 - 1/2)^2 + 2r/sig^2).
*/
double PerpetualAmericanOption::Y2() const {
    const double sig2 = m_params.sig * m_params.sig;
    const double a = m_params.b / sig2 - 0.5;
    return 0.5 - m_params.b / sig2 - std::sqrt(a * a + 2.0 * m_params.r / sig2);
}

/*
Prices the option.
*/
double PerpetualAmericanOption::Price() const {

    const double S = m_params.S;
    const double K = m_params.K;

    switch (m_type) {
    case OptionType::Call: {
        const double y1 = Y1();
        if (!(y1 > 1.0)) {
            throw std::domain_error("PerpetualAmericanOption: call has no "
                                    "finite price unless b < r.");
        }
        // At or above the exercise boundary the call is exercised immediately
        const double boundary = K * y1 / (y1 - 1.0);
        if (S >= boundary) {
            return S - K;
        }
        return K / (y1 - 1.0) * std::pow((y1 - 1.0) / y1 * S / K, y1);
    }
    case OptionType::Put: {
        const double y2 = Y2();
        if (!(y2 < 0.0)) {
            throw std::domain_error("PerpetualAmericanOption: put has no "
                                    "finite price unless r > 0.");
        }
        // At or below the exercise boundary the put is exercised immediately
        const double boundary = K * y2 / (y2 - 1.0);
        if (S <= boundary) {
            return K - S;
        }
        return K / (1.0 - y2) * std::pow((y2 - 1.0) / y2 * S / K, y2);
    }
    default:
        throw std::logic_error(
            "PerpetualAmericanOption::Price: unknown OptionType.");
    }
}

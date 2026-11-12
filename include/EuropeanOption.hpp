/*
Level 9: Introductory Computational Finance
Group A & B: Exact Pricing Methods
*/
#ifndef EUROPEAN_OPTION_HPP
#define EUROPEAN_OPTION_HPP

#include "Option.hpp"

/*
Representation of an European Option.
*/
class EuropeanOption : public Option {

  protected:
    /*
    Calculates d1
    */
    double D1() const;

    /*
    Calculates d2
    */
    double D2() const;

    /*
    Cost-of-carry factor e^{(b-r)T}.
    */
    double Carry() const;

    /*
    Discount factor e^{-rT}.
    */
    double Discount() const;

    /*
    Right-hand side of put-call parity: C - P = Se^{(b-r)T} - Ke^{-rT}.
    */
    static double ParityGap(const OptionParameters& params);

  public:
    /*
    Constructor which requires option pricing parameters.
    */
    EuropeanOption(const OptionParameters& params, OptionType type);

    /*
    Prices the option.
    */
    double Price() const override;

    /*
    Price of the opposite option type (put for a call, call for a put), computed
    from this option's price using put-call parity.
    */
    double ParityPrice() const;

    /*
    Checks whether a call price and a put price satisfy put-call parity for the
    given parameters, to within an absolute tolerance.
    */
    static bool SatisfiesParity(double call, double put,
                                const OptionParameters& params,
                                double tol = 1e-6);

    /*
    Delta: sensitivity of the price to the underlying, dV/dS.
    */
    double Delta() const;

    /*
    Gamma: sensitivity of Delta to the underlying, d2V/dS2. Same for calls and
    puts.
    */
    double Gamma() const;

    /*
    Vega: sensitivity of the price to volatility, dV/dsig. Same for calls and
    puts.
    */
    double Vega() const;

    /*
    Theta: sensitivity of the price to the passage of time, -dV/dT.
    */
    double Theta() const;
};

#endif
